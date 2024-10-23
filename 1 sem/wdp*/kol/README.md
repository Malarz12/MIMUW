# Simulation of a Day in the Office

This project implements a simulation of events in a public office, where customers queue up at service windows. The project was created as part of a programming assignment in C++ (C++17 standard). The program simulates handling customers throughout a workday, considering various events that may occur, such as queue changes, service window closures, and priority handling.

## Features
- **Opening the Office**: Initializes the office with a specified number of service windows.
- **New Customer**: A new customer joins the queue at a selected window.
- **Customer Numbering**: Each customer is assigned a unique number upon arrival.
- **Serving a Customer**: The customer at the front of the queue is served and removed from the queue.
- **Queue Change**: A customer can switch from one queue to another.
- **Window Closure**: Customers from a closed window are transferred to another window.
- **Priority Handling**: A special fast-track service allows quick handling of a group of customers from a specific queue.
- **Reversing the Queue**: The queue can be reversed at a specific window by the office head.
- **Closing the Office**: At the end of the day, all remaining customers in the queues are served, and the memory allocated for them is freed.

The solution efficiently handles the queues using doubly linked lists, ensuring that all operations are performed with time complexity proportional to the size of the input and output, except for opening and closing the office, which take additional time O(m).

The program is also designed to handle memory management properly, with no memory leaks, as validated using tools like `valgrind`.

## Compilation
To compile the project, use the following command:

```bash
g++ @opcjeCpp main.cpp kol.cpp -o main.e
