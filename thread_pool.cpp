#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>

static void* worker_loop(void* arg){
  ThreadPool* pool = (ThreadPool*)arg;

  while (!pool->stop){
    // wait for task
    sem_wait(&pool->sem_tasks);

    if(pool->stop) break;

    // pop from queue
    int fd = pool->tasks[pool->task_start];
    pool->task_start = (pool->task_start + 1) % MAX_TASKS;

    sem_post(&pool->sem_slots);

    // handle client                                                                               
    handle_client(fd);
    
  }
  return NULL;
}

void pool_init(ThreadPool* pool, int num_threads){
  // populate struct                                                                                
  pool->num_threads = num_threads;
  pool->task_start = 0;
  pool->task_end = 0;
  pool->stop = false;

  pool->threads	= (pthread_t*)malloc(sizeof(pthread_t) * num_threads);

  // init semaphores                                                                                
  sem_init(&pool->sem_tasks, 0, 0);
  sem_init(&pool->sem_slots, 0, MAX_TASKS);

  // launch threads                                                                                 
  for(int i = 0; i < num_threads; i++){
    pthread_create(&pool->threads[i], NULL, worker_loop, pool);
  }
}

void pool_destroy(ThreadPool* pool){
  // notify threads to stop
  pool->stop = true;

  // wake up all sleeping workers
  for (int i = 0; i < NUM_THREADS; i++){
    sem_post(&pool->sem_tasks);
  }

  // wait for all to exit
  for (int i = 0; i < pool->num_threads; i++){
    pthread_join(pool->threads[i], NULL);
  }

  sem_destroy(&pool->sem_tasks);
  sem_destroy(&pool->sem_slots);

  free(pool->threads);
}

void pool_enqueue(ThreadPool* pool, int client_fd){
  sem_wait(&pool->sem_slots);

  pool->tasks[pool->task_end] = client_fd;
  pool->task_end = (pool->task_end + 1) % MAX_TASKS;

  sem_post(&pool->sem_tasks);
}
