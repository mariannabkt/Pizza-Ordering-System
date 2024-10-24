# Pizza Ordering and Delivery System

## Overview
This project implements a multi-threaded application simulating a pizza restaurant's order and delivery process. It effectively utilizes POSIX threads to manage concurrent operations, ensuring smooth interactions between waitstaff, cooks, ovens, and delivery personnel.

## Features
- **Multi-threaded Operations**: Simulates concurrent processing for handling multiple customer orders.
- **Resource Synchronization**: Uses mutexes and condition variables to manage access to shared resources, ensuring safe interactions among different roles (waitstaff, cooks, ovens, delivery personnel).
- **Dynamic Order Processing**: Customers can place orders at random intervals, with various pizza types and payment processes.
- **Statistical Reporting**: At the end of the simulation, the application reports total sales, successful and failed orders, and average delivery times.

## Technologies Used
- C
- POSIX Threads (pthreads)

## Getting Started
To compile and run the project, follow these steps:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/mariannabkt/Pizza-Ordering-System.git
   cd Pizza-Ordering-System
2. **Compile the code: Make sure to include the -pthread flag when compiling**:
   ```bash
   gcc -o pizza_ordering_system main.c -pthread
4. Run the application: Provide the number of customers and a random seed as parameters:
   ```bash
    ./pizza_ordering_system <number_of_customers> <random_seed>

## Usage
- The application starts by simulating customer calls for orders.
- Each customer can place a random number of pizza orders.
- The order processing includes payment handling, cooking, baking, and delivery.
- Results and statistics are displayed in the console after all orders are processed.
