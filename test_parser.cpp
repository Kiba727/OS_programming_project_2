/**
 * @file test_parser.cpp
 * @brief Test driver for http_parser.cpp
 * Compile with: g++ -o test_parser test_parser.cpp http_parser.cpp
 */

#include "http_parser.h"
#include <iostream>
#include <time.h>

int main() {
    std::cout << "=== Starting HTTP Parser Test ===\n";
    std::cout << "Note: current implementation sleeps for 10 seconds.\n";

    // Use a dummy FD, since the current handle_client doesn't read/write
    int dummy_fd = 999; 

    time_t start = time(NULL);
    
    std::cout << "[Test] Calling handle_client...\n";
    handle_client(dummy_fd);
    
    time_t end = time(NULL);
    double elapsed = difftime(end, start);

    std::cout << "[PASS] handle_client returned after " << elapsed << " seconds.\n";

    return 0;
}