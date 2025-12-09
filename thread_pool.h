#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "http_parser.h"

// number of worker threads
const int NUM_THREADS = 10;
// max tasks in queue
const int MAX_TASKS = 50;

/** 
  * @struct ThreadPool
  * @brief Structure representing a thread pool
  * @var threads      Array of thread handles
  * @var num_threads  Number of worker threads
  * @var tasks        Circular buffer (array) of client file descriptors
  * @var task_start   Index of head of the circular queue
  * @var task_end     Index of tail of the circular queue
  * @var sem_tasks    Semaphore counting the number of pending tasks
  * @var sem_slots    Semaphore counting the number of free slots in the queue
  * @var stop         Flag to signal threads to stop processing and exit
*/
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
 * @brief Sets up struct fields, allocates memory, and creates worker threads
 *
 * @param pool        Pointer to the ThreadPool structure to initialize.
 * @param num_threads The number of worker threads to launch.
 *
 * @return 0 if successful, -1 on error
 */
int pool_init(ThreadPool* pool, int num_threads);

/**
 * @brief Shuts down the thread pool and cleans up resources.
 * All threads are signaled to stop and memory is reclaimed
 *
 * @param pool Pointer to the ThreadPool to destroy.
 *
 * @return void
 */
void pool_destroy(ThreadPool* pool);

/**
 * @brief Adds a new client file descriptor to the processing queue.
 * Enqueues a new task (client connection) to the task queue
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