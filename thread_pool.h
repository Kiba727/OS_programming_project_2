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

void pool_init(ThreadPool* pool, int num_threads);
void pool_destroy(ThreadPool* pool);
void pool_enqueue(ThreadPool* pool, int client_fn);
  
