#ifndef PIZZA_SIMULATION_H
#define PIZZA_SIMULATION_H

//Restaurant's assets
#define Ntel 2
#define Ncook 2
#define Noven 10
#define Npacker 2
#define Ndeliverer 10

//next customer call time period
#define Torderlow 1
#define Torderhigh 5

//pizzas per order
#define Norderlow 1
#define Norderhigh 5

//type of pizza probabilities
#define Pm 45
#define Pp 35
#define Ps 20

//payment accepted period
#define Tpaymentlow 1
#define Tpaymenthigh 3

//probability of failed transaction
#define Pfail 5

//Price per pizza
#define Cm 12
#define Cp 14
#define Cs 15

//Time to prepare,bake and pack a pizza
#define Tprep 1
#define Tbake 10
#define Tpack 1

//Time to deliver an order
#define Tdellow 10
#define Tdelhigh 15

#endif