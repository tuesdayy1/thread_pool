# Thread Pool Using WinAPI

A small educational C/C++ project demonstrating multithreaded task processing using the Windows API.

## Description

The program implements a simple **Producer–Consumer** model:

- the main thread creates tasks and places them into a shared buffer;
- multiple worker threads retrieve and process tasks;
- access to the shared buffer is synchronized using a `Mutex`;
- two semaphores control the number of empty and occupied buffer slots.

Default configuration:

- Buffer size: `5`
- Worker threads: `3`
- Total tasks: `15`

## Task Structure

Each task contains:

```cpp
typedef struct {
    int task_id;
    int operation;
    int data;
} Task;
```

The `operation` field determines which operation is performed:

- `1` — square the value;
- `2` — increment the value by one.

## Synchronization

The program uses several WinAPI synchronization objects:

- `Mutex` — provides exclusive access to the shared buffer;
- `hSemEmpty` — tracks the number of available buffer slots;
- `hSemFull` — tracks the number of tasks available for processing.

The shared buffer works as a circular queue. The `write_index` and `read_index` values wrap around when they reach the end of the buffer.

## How It Works

The main thread:

1. Creates the mutex and semaphores.
2. Starts the worker threads.
3. Generates 15 tasks.
4. Waits for an available slot in the buffer.
5. Adds each task to the shared buffer.
6. Stops producing tasks after all tasks have been added.
7. Waits for all worker threads to finish.
8. Releases the allocated system resources.

Each worker thread:

1. Waits for a task to become available.
2. Locks the mutex.
3. Retrieves a task from the shared buffer.
4. Releases the mutex and signals that a buffer slot is available.
5. Processes the task.
6. Prints the result to the console.
7. Terminates when there are no more tasks to process.

## Build and Run

This project is designed for **Windows** because it uses WinAPI.

### MinGW

Compile the program with:

```bash
g++ thread_pool.cpp -o thread_pool.exe
```

Run it with:

```bash
thread_pool.exe
```

The source file can also be added to a C/C++ console project in Visual Studio and compiled using the standard Visual Studio build tools.

## Example Output

The exact order of worker messages may vary between runs because the tasks are processed concurrently.

```text
[MAIN] Starting.
[WORKER 1] Started task 1 (Op: 2, Data: 5).
[WORKER 2] Started task 2 (Op: 1, Data: 4).
[WORKER 1] Finished task 1. Result: 6.
[WORKER 2] Finished task 2. Result: 16.
...
[MAIN] All workers shut down.
```

The task data is generated randomly, so the values and execution order may differ between runs.

## Purpose

The project demonstrates the basic concepts of multithreaded programming:

- creating and managing threads;
- synchronizing access to shared resources;
- using mutexes and semaphores;
- implementing a circular buffer;
- applying the Producer–Consumer pattern.
