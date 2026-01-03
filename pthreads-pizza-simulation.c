#include "pthreads-pizza-simulation.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

//restaurant set-up

int Ntel_available=Ntel;
int Ncook_available=Ncook;
int Noven_available=Noven;
int Npacker_available=Npacker;
int Ndeliverer_available=Ndeliverer;

//output essentials
int income=0;
int Npizzas_m=0;
int Npizzas_p=0;
int Npizzas_s=0;
int successful_orders=0;
int failed_orders=0;
int total_service_time=0;
int max_service_time=0;
int cool_time=0;
int max_cool_time=0;

//Mutexes 
pthread_mutex_t mutex_screen = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_tel = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_tel = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_cook = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_cook = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_oven = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_oven = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_packer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_packer = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_deliverer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_deliverer = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_stats = PTHREAD_MUTEX_INITIALIZER;

//order(customer) struct
typedef struct order_class{
    int id;
    int success;
    int total_time;
    int cool_time;
    unsigned int seedp;
    struct timespec start_time;
    struct timespec bake_done_time;
}order_class;

//rand_r
int rand_wlimits(unsigned int *seedp,int dt1,int dt2){
    return rand_r(seedp)%(dt2-dt1+1)+dt1;
}

int timespec_diff_sec(struct timespec *start,struct timespec *end){
    return (int)((*end).tv_sec - (*start).tv_sec);
}

void *order_func(void* arg){
    order_class* order =(order_class*) arg;
    int id= (*order).id;
    unsigned int seed=(*order).seedp;
    clock_gettime(CLOCK_REALTIME, &(*order).start_time);

    //incoming call management
    pthread_mutex_lock(&mutex_tel);
    while(Ntel_available==0){
        pthread_cond_wait(&cond_tel,&mutex_tel);
    }
    Ntel_available--;
    pthread_mutex_unlock(&mutex_tel);

    int pizzas=rand_wlimits(&seed,Norderlow,Norderhigh);
    int payment_time= rand_wlimits(&seed,Tpaymentlow,Tpaymenthigh);
    sleep(payment_time);

    //failed transaction
    int fail_chance=rand_wlimits(&seed,1,100);//%percentage
    if(fail_chance>Pfail){
        // Successful order
        pthread_mutex_lock(&mutex_screen);
        printf("Order with id %d registered\n", id);
        pthread_mutex_unlock(&mutex_screen);
    }
    else{
        //show message and increase available telephones
        pthread_mutex_lock(&mutex_screen);
        printf("order with id %d failed \n",id);
        pthread_mutex_unlock(&mutex_screen);

        pthread_mutex_lock(&mutex_tel);
        Ntel_available++;
        pthread_cond_signal(&cond_tel);
        pthread_mutex_unlock(&mutex_tel);

        //update stats
        pthread_mutex_lock(&mutex_stats);
        failed_orders++;
        pthread_mutex_unlock(&mutex_stats);

        //free order
        free(order);
        pthread_exit(NULL);
    }
    //count each type of pizza and update
    int charge=0;
    for(int i=0;i<pizzas;i++){
        int pizza_limits=rand_wlimits(&seed,1,100);
        if(pizza_limits<=Pm){
            charge+=Cm;
            pthread_mutex_lock(&mutex_stats);
            Npizzas_m++;
            pthread_mutex_unlock(&mutex_stats);
        }else if(pizza_limits<=Pm+Pp){
            charge+=Cp;
            pthread_mutex_lock(&mutex_stats);
            Npizzas_p++;
            pthread_mutex_unlock(&mutex_stats);
        }else{
            charge+=Cs;
            pthread_mutex_lock(&mutex_stats);
            Npizzas_s++;
            pthread_mutex_unlock(&mutex_stats);
        }
    }

    //increase available telephones
    pthread_mutex_lock(&mutex_tel);
    Ntel_available++;
    pthread_cond_signal(&cond_tel);
    pthread_mutex_unlock(&mutex_tel);

    //increase income
    pthread_mutex_lock(&mutex_stats);
    income+=charge;
    pthread_mutex_unlock(&mutex_stats);

    //cooks pizzas
    pthread_mutex_lock(&mutex_cook);
    while(Ncook_available==0){
        pthread_cond_wait(&cond_cook,&mutex_cook);
    }
    Ncook_available--;
    pthread_mutex_unlock(&mutex_cook);
    sleep(Tprep*pizzas);

    //bake pizzas
    pthread_mutex_lock(&mutex_oven);
    while(Noven_available<pizzas){//wait until we have enough ovens
        pthread_cond_wait(&cond_oven,&mutex_oven);
    }
    Noven_available-=pizzas;
    pthread_mutex_unlock(&mutex_oven);

    pthread_mutex_lock(&mutex_cook);
    Ncook_available++;//pizzas are in the oven so the cook returns to preparing the next order
    pthread_cond_signal(&cond_cook);
    pthread_mutex_unlock(&mutex_cook);

    sleep(Tbake);
    clock_gettime(CLOCK_REALTIME,&(*order).bake_done_time);

    //Packs each pizza
    pthread_mutex_lock(&mutex_packer);
    while(Npacker_available==0){
        pthread_cond_wait(&cond_packer,&mutex_packer);
    }
    Npacker_available--;
    pthread_mutex_unlock(&mutex_packer);
    sleep(Tpack*pizzas);//end of packing

    pthread_mutex_lock(&mutex_oven);
    Noven_available+=pizzas;  //ovens are available again
    pthread_cond_broadcast(&cond_oven);
    pthread_mutex_unlock(&mutex_oven);

    pthread_mutex_lock(&mutex_packer);
    Npacker_available++;//packer is available again
    pthread_cond_signal(&cond_packer);
    pthread_mutex_unlock(&mutex_packer);

    //show how much it took to prepare the order
    struct timespec after_prep;
    clock_gettime(CLOCK_REALTIME,&after_prep);
    pthread_mutex_lock(&mutex_screen);
    printf("Order %d was prepared in %d minutes \n",id, timespec_diff_sec(&(*order).start_time, &after_prep));
    pthread_mutex_unlock(&mutex_screen);

    //order delivery
    pthread_mutex_lock(&mutex_deliverer);
    while(Ndeliverer_available==0){
        pthread_cond_wait(&cond_deliverer,&mutex_deliverer);
    }
    Ndeliverer_available--;
    pthread_mutex_unlock(&mutex_deliverer);
    int Tdelivery=rand_wlimits(&seed,Tdellow,Tdelhigh);
    sleep(Tdelivery); //delivery

    //count the time until the pizza was delivered
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME,&end_time);

    int total=timespec_diff_sec(&(*order).start_time,&end_time);;
    int cool=timespec_diff_sec(&(*order).bake_done_time,&end_time);

    pthread_mutex_lock(&mutex_screen);
    printf("Order %d delivered in %d minutes \n",id,total);
    pthread_mutex_unlock(&mutex_screen);

    pthread_mutex_lock(&mutex_stats);
    successful_orders++;
    total_service_time+=total;
    cool_time+=cool;
    if(total>max_service_time){
        max_service_time=total;
    }
    if(cool>max_cool_time){
        max_cool_time=cool;
    }
    pthread_mutex_unlock(&mutex_stats);
    sleep(Tdelivery);

    pthread_mutex_lock(&mutex_deliverer);
    Ndeliverer_available++;//deliverer is available again
    pthread_cond_signal(&cond_deliverer);
    pthread_mutex_unlock(&mutex_deliverer);

    free(order);
    pthread_exit(NULL);
}

int main(int argc,char* argv[]){
    if (argc!=3){
        printf("Insert only Number of Customers and seed");
        return 1;
    }

    int Ncust=atoi(argv[1]);
    unsigned int seed_main=atoi(argv[2]);

    pthread_t threads[Ncust];

    //starts creating orders
    for(int i=0;i<Ncust;i++){
        
        if (i > 0) {
            int tel_waiting_time=rand_wlimits(&seed_main,Torderlow,Torderhigh);
            sleep(tel_waiting_time);
        }

        order_class* ord=malloc(sizeof(order_class));
        (*ord).id=i+1;
        (*ord).seedp=seed_main+i;

        pthread_create(&threads[i],NULL,order_func,ord);
    }
    for(int i=0;i<Ncust;i++){
        pthread_join(threads[i],NULL);
    }

    printf("\nStats \n");
    printf("Total income:%d euro\n",income);
    printf("Sold:%d margaritas, %d peperonies, %d special \n",Npizzas_m,Npizzas_p,Npizzas_s);
    printf("Successful orders:%d\n",successful_orders);
    printf("Failed orders:%d\n",failed_orders);

    if(successful_orders>0){
        printf("Average servicing time: %.2f minutes\n",(float)total_service_time/successful_orders);
        printf("Max servicing time: %d minutes \n",max_service_time);
        printf("Average cooling time:%.2f minutes\n",(float)cool_time/successful_orders);
        printf("Max cooling time:%d minutes \n",max_cool_time);
    }
    return 0;
}