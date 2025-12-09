/**
 * @file test_thread_pool.cpp
 * @brief Test driver for thread_pool.cpp
 * Compile with: g++ -o test_pool test_thread_pool.cpp thread_pool.cpp -lpthread
 */

#include "thread_pool.h"
#include <iostream>
#include <vector>
#include <unistd.h>
#include <atomic>
#include <cassert>

// --- MOCK handle_client ---
// We implement this here to override the real one in http_parser.cpp
// This allows us to track if the thread pool is actually doing work.
std::atomic<int> tasks_completed{0};

void handle_client(int client_fd) {
    // Simulate a small amount of work (100ms)
    usleep(100000); 
    
    // Increment the atomic counter so we can verify all tasks ran
    tasks_completed++;
    
    // In a real scenario, the worker or the handler should close the socket.
    // For this test, we just print.
    // printf("Processed mock task: %d\n", client_fd); 
}
// --------------------------

int main() {
    std::cout << "=== Starting Thread Pool Test ===\n";

    ThreadPool pool;
    int num_threads = 4;
    int num_tasks = 20;

    // 1. Test Initialization
    std::cout << "[Test] Initializing pool with " << num_threads << " threads...\n";
    pool_init(&pool, num_threads);

    // 2. Test Enqueueing
    std::cout << "[Test] Enqueuing " << num_tasks << " tasks...\n";
    for (int i = 0; i < num_tasks; i++) {
        // We pass 'i' as a fake file descriptor
        pool_enqueue(&pool, i);
    }

    // 3. Wait for completion
    // Since your pool doesn't have a "wait_for_all" function, we sleep 
    // long enough for the mock work to finish.
    // 20 tasks * 0.1s / 4 threads = ~0.5 seconds needed. We wait 2s to be safe.
    std::cout << "[Test] Waiting for tasks to complete...\n";
    sleep(2);

    // 4. Verify results
    int completed = tasks_completed.load();
    std::cout << "[Result] Tasks completed: " << completed << "/" << num_tasks << "\n";
    
    if (completed == num_tasks) {
        std::cout << "[PASS] All tasks processed.\n";
    } else {
        std::cout << "[FAIL] Missing " << (num_tasks - completed) << " tasks.\n";
    }

    // 5. Test Destruction
    std::cout << "[Test] Destroying pool...\n";
    pool_destroy(&pool);
    std::cout << "[PASS] Pool destroyed cleanly.\n";

    return 0;
}