# POSIX Threads Pizza Simulation

A high-performance C simulation of a pizza restaurant's order and delivery pipeline, utilizing **multithreading** and **synchronization primitives**.

## 🚀 Overview
This project simulates a real-time environment where multiple customer orders (threads) compete for limited resources. It implements complex synchronization logic to manage the lifecycle of an order from call reception to final delivery.

## 🛠️ Technical Features
- **Thread Management:** Each order is a standalone thread (`pthread_create`), ensuring concurrent processing.
- **Resource Synchronization:** Uses **Mutexes** for mutual exclusion and **Condition Variables** for process synchronization (e.g., waiting for available ovens or deliverers).
- **Dynamic Resource Allocation:** - **Telephonists:** 2 stations for order entry.
    - **Cooks & Ovens:** Parallel baking logic where one cook handles an order until it enters the ovens.
    - **Logistics:** Managed packing and delivery stages with return-trip logic for drivers.
- **Statistical Analysis:** Real-time tracking of average/maximum service and cooling times.

## 💻 How to Run
Compile using the `pthread` library:
```bash
gcc -Wall -pthread pthreads-pizza-simulation.c -o pizza_sim
