/**
 * @file test_thread_pool.cpp
 * @brief Simple tests for thread_pool.cpp (no atomic, for experiment)
 */

#include "thread_pool.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

// We use a fake handle_client() so we don't need real networking
//
// The real handle_client() is in http_parser.cpp, but for testing the
// thread pool we just need "some work" to be done by each task.

int tasks_completed = 0;

void handle_client(int client_fd) {
    // Pretend to do some work for 0.1 seconds
    usleep(100000);

    // Count that this "task" finished
    tasks_completed++;

    // In the real code, worker_loop() closes the socket.
    // Here client_fd is just a fake number, so we do nothing with it.
}

// TEST 1: Create and destroy the pool
bool test_init_destroy() {
    std::cout << "\n=== Test 1: Init and Destroy ===\n";

    ThreadPool pool;
    int num_threads = 4;

    std::cout << "[Test] Creating pool with " << num_threads << " threads...\n";
    pool_init(&pool, num_threads);

    // Give threads time to start
    usleep(100000);

    std::cout << "[Test] Destroying pool...\n";
    pool_destroy(&pool);

    std::cout << "[PASS] Pool created and destroyed without crashing\n";
    return true;
}

// TEST 2: Process a bunch of tasks
bool test_process_tasks() {
    std::cout << "\n=== Test 2: Process Multiple Tasks ===\n";

    tasks_completed = 0;

    ThreadPool pool;
    int num_threads = 4;
    int num_tasks   = 20;

    pool_init(&pool, num_threads);

    std::cout << "[Test] Enqueuing " << num_tasks << " tasks...\n";

    // Use fake "FDs" starting at 1000 (so we don't touch 0,1,2)
    for (int i = 0; i < num_tasks; i++) {
        pool_enqueue(&pool, 1000 + i);
    }

    int completed = tasks_completed;  
    std::cout << "[Result] Tasks completed: " << completed << "/" << num_tasks << "\n";

    bool passed = (completed == num_tasks);
    if (passed) {
        std::cout << "[PASS] All tasks were handled by the pool\n";
    } else {
        std::cout << "[FAIL] Some tasks did not run (or the counter missed some)\n";
    }

    pool_destroy(&pool);
    return passed;
}

// TEST 3: Fill the queue up to MAX_TASKS
bool test_queue_capacity() {
    std::cout << "\n=== Test 3: Queue Capacity ===\n";

    tasks_completed = 0;

    ThreadPool pool;
    int num_threads = 2;
    int num_tasks   = MAX_TASKS; 

    pool_init(&pool, num_threads);

    std::cout << "[Test] Enqueuing " << num_tasks << " tasks (MAX_TASKS=" << MAX_TASKS << ")...\n";

    for (int i = 0; i < num_tasks; i++) {
        pool_enqueue(&pool, 1000 + i);

        if ((i + 1) % 10 == 0) {
            std::cout << "  Enqueued " << (i + 1) << " tasks\n";
        }
    }

    std::cout << "[Test] Waiting for all tasks to finish...\n";
    sleep(5);

    int completed = tasks_completed; 
    std::cout << "[Result] Tasks completed: " << completed << "/" << num_tasks << "\n";

    bool passed = (completed == num_tasks);
    if (passed) {
        std::cout << "[PASS] Queue handled MAX_TASKS correctly\n";
    } else {
        std::cout << "[FAIL] Some tasks were lost or never executed (or counter raced)\n";
    }

    pool_destroy(&pool);
    return passed;
}

// TEST 4: Destroy the pool while work is still running
bool test_immediate_shutdown() {
    std::cout << "\n=== Test 4: Immediate Shutdown ===\n";

    tasks_completed = 0;

    ThreadPool pool;
    int num_threads = 2;
    int num_tasks   = 10;

    pool_init(&pool, num_threads);

    std::cout << "[Test] Enqueuing " << num_tasks << " tasks...\n";
    for (int i = 0; i < num_tasks; i++) {
        pool_enqueue(&pool, 1000 + i);
    }

    std::cout << "[Test] Calling pool_destroy() right away (no waiting)...\n";
    pool_destroy(&pool);

    int completed = tasks_completed;  
    std::cout << "[Result] Tasks completed before shutdown: "
              << completed << "/" << num_tasks << "\n";

    std::cout << "[PASS] Pool destroyed cleanly even with work in progress\n";
    return true;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "       Thread Pool Test Suite\n";
    std::cout << "========================================\n";

    int passed = 0;
    int total  = 4;

    if (test_init_destroy())        passed++;
    if (test_process_tasks())       passed++;
    if (test_queue_capacity())      passed++;
    if (test_immediate_shutdown())  passed++;

    std::cout << "\n========================================\n";
    std::cout << "           Test Summary\n";
    std::cout << "========================================\n";
    std::cout << "Passed: " << passed << "/" << total << "\n";

    if (passed == total) {
        std::cout << "\n[SUCCESS] All tests passed!\n";
        return 0;
    } else {
        std::cout << "\n[FAILURE] Some tests failed. Check above output.\n";
        return 1;
    }
}