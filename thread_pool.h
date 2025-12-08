#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "http_parser.h"

const int NUM_THREADS = 10;
const int MAX_TASKS = 50;

typedef struct{
    pthread_t* threads;
    int num_threads;

    int tasks[MAX_TASKS];
    int task_start = 0;
    int task_end = 0;

    sem_t sem_tasks;
    sem_t sem_slots;

    bool stop;

  } ThreadPool;

/**
 * @brief Initializes the thread pool and spawns worker threads.
 *
 * This function sets up the internal state of the thread pool, initializes
 * synchronization primitives (semaphores), and creates the specified
 * number of worker threads.
 *
 * @pre The `pool` structure must be allocated by the caller before passing it in.
 * @pre `MAX_TASKS` must be defined globally.
 *
 * @param pool        Pointer to the ThreadPool structure to initialize.
 * @param num_threads The number of worker threads to launch.
 *
 * @return void
 */
void pool_init(ThreadPool* pool, int num_threads);

/**
 * @brief Shuts down the thread pool and cleans up resources.
 *
 * This function performs a graceful shutdown:
 * 1. Sets a global stop flag.
 * 2. Wakes up all sleeping workers so they can read the stop flag.
 * 3. Joins (waits for) all threads to ensure they exit safely.
 * 4. Destroys the semaphores and frees the thread handle array.
 *
 * @warning Ensure no new tasks are being enqueued while this function is running.
 *
 * @param pool Pointer to the ThreadPool structure to destroy.
 *
 * @return void
 */
void pool_destroy(ThreadPool* pool);

/**
 * @brief Adds a new client file descriptor to the processing queue.
 *
 * This function acts as the "Producer". It places a file descriptor into
 * the circular buffer. If the buffer is currently full (reached MAX_TASKS),
 * this function will block (wait) until a slot becomes available.
 *
 * Once the task is added, it signals a sleeping worker thread to wake up
 * and process the request.
 *
 * @param pool      Pointer to the initialized ThreadPool structure.
 * @param client_fd The file descriptor (int) representing the client connection
 * or task to be processed.
 *
 * @return void
 */
void pool_enqueue(ThreadPool* pool, int client_fd);