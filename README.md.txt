# POSIX Threads Pizza Simulation

A high-performance C simulation of a pizza restaurant's order and delivery pipeline, utilizing **multithreading** and **synchronization primitives**.

## 🚀 Overview
This project simulates a real-time environment where multiple customer orders (threads) compete for limited resources. It implements complex synchronization logic to manage the lifecycle of an order from call reception to final delivery.

## 🛠️ Technical Features
- [cite_start]**Thread Management:** Each order is a standalone thread (`pthread_create`), ensuring concurrent processing[cite: 50, 51].
- [cite_start]**Resource Synchronization:** Uses **Mutexes** for mutual exclusion and **Condition Variables** for process synchronization (e.g., waiting for available ovens or deliverers)[cite: 7, 53, 54].
- [cite_start]**Dynamic Resource Allocation:** - **Telephonists:** 2 stations for order entry[cite: 30].
    - [cite_start]**Cooks & Ovens:** Parallel baking logic where one cook handles an order until it enters the ovens[cite: 21, 22].
    - [cite_start]**Logistics:** Managed packing and delivery stages with return-trip logic for drivers[cite: 25, 26].
- [cite_start]**Statistical Analysis:** Real-time tracking of average/maximum service and cooling times[cite: 48, 49].

## 💻 How to Run
Compile using the `pthread` library:
```bash
gcc -Wall -pthread pthreads-pizza-simulation.c -o pizza_sim