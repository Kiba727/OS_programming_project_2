/**
 * @file test_parser.cpp
 * @brief test driver for http_parser.cpp
 * 
 * This test creates socket connections to test HTTP parsing.
 * We use socketpair() to create two connected sockets,
 * then use fork() to split into two processes:
 *   - Child process = Client (sends HTTP requests)
 *   - Parent process = Server (runs handle_client to parse requests)
 *  Type make test_parser to compile
 * 
 */

#include "http_parser.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>

bool test_valid_get_request() {
    std::cout << "\n=== Test 1: Valid GET Request ===\n";
    
    // socketpair() creates two connected sockets
    // sv[0] and sv[1] can send/receive data to each other
    // AF_UNIX means local communication (not over network)
    // SOCK_STREAM means TCP reliable connection
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        std::cerr << "[FAIL] Could not create socket pair\n";
        return false;
    }
    
    // fork() splits our process into two:
    //   - Parent process (pid > 0): Will act as the server
    //   - Child process (pid == 0): Will act as the client
    // Both processes continue from this point, but pid tells them who they are
    pid_t pid = fork();
    
    if (pid == 0) {
        // CHILD PROCESS (CLIENT SIDE)
        // The child will send an HTTP request and check the response
        
        close(sv[1]); // We're the client, so close the server's socket (sv[1])
        
        // Create a proper HTTP GET request as a string
        // \r\n is "carriage return + newline" (required by HTTP protocol)
        // Empty line (\r\n\r\n) signals end of headers
        const char* request = 
            "GET / HTTP/1.1\r\n"           // Request line: method, path, version
            "Host: localhost\r\n"           // Required header in HTTP/1.1
            "\r\n";                         // Empty line = end of headers
        
        // send() writes data to the socket
        send(sv[0], request, strlen(request), 0);
        
        // Now wait for the server's response
        // recv() reads data from the socket 
        char buf[4096];  // Buffer to store the response
        ssize_t n = recv(sv[0], buf, sizeof(buf)-1, 0);
        
        // Check if we got a valid response
        bool passed = false;
        if (n > 0) {
            buf[n] = '\0';  // Null-terminate the string so we can use strstr()
            
            // strstr() searches for a substring within a string
            // We're looking for "HTTP/1.0" to confirm it's a valid HTTP response
            if (strstr(buf, "HTTP/1.0")) {
                if (strstr(buf, "200 OK")) {
                    // 200 = Success (file was found and sent)
                    std::cout << "[PASS] Received 200 OK response\n";
                    passed = true;
                } else if (strstr(buf, "404")) {
                    // 404 = Not Found (www/index.html doesn't exist)
                    // Expected if www/ directory is missing
                    std::cout << "[PASS] Received 404 (www/index.html doesn't exist - expected)\n";
                    passed = true;
                } else {
                    // Some other HTTP response (still valid)
                    std::cout << "[INFO] Received valid HTTP response\n";
                    passed = true;
                }
            } else {
                std::cout << "[FAIL] Invalid HTTP response\n";
            }
        } else {
            std::cout << "[FAIL] No response received\n";
        }
        
        close(sv[0]);  // Close our socket
        
        // exit() terminates the child process
        // Exit code 0 = success, 1 = failure
        // The parent can read this exit code to know if test passed
        exit(passed ? 0 : 1);
        
    } else {
        // PARENT PROCESS (SERVER SIDE)
        // The parent will act as the server and process the request
        
        close(sv[0]); // We're the server, so close the client's socket (sv[0])
        
        // Call handle_client to process the request from the client
        // handle_client is expected to:
        // - Read the HTTP request from sv[1]
        // - Parse it (extract method, URI, etc.)
        // - Send back an HTTP response
        handle_client(sv[1]);
        
        close(sv[1]);  // Close our socket
        
        // wait() makes the parent wait for the child to finish
        // This prevents "zombie processes" and lets us get the child's exit code
        int status;
        wait(&status);
        
        // WIFEXITED checks if child exited normally (not crashed)
        // WEXITSTATUS extracts the exit code (0 or 1)
        // Return true if child exited with code 0 (success)
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

bool test_invalid_method() {
    std::cout << "\n=== Test 2: Invalid Method (POST) ===\n";
    
    // Create connected sockets
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        return false;
    }
    
    // Split into client and server processes
    pid_t pid = fork();
    
    if (pid == 0) {
        // CHILD PROCESS (CLIENT)
        close(sv[1]);
        
        // Send a POST request
        // Server only accepts GET, so this should return 405 error
        const char* request = 
            "POST /upload HTTP/1.1\r\n"     // POST method (not allowed)
            "Content-Length: 0\r\n"         // Header (POST usually has a body)
            "\r\n";                         // End of headers
        
        send(sv[0], request, strlen(request), 0);
        
        // Wait for response
        char buf[4096];
        ssize_t n = recv(sv[0], buf, sizeof(buf)-1, 0);
        
        bool passed = false;
        if (n > 0) {
            buf[n] = '\0';
            
            // Look for "405 Method Not Allowed" in the response
            if (strstr(buf, "405")) {
                std::cout << "[PASS] Received 405 Method Not Allowed\n";
                passed = true;
            } else {
                std::cout << "[FAIL] Expected 405, got different response\n";
            }
        }
        
        close(sv[0]);
        exit(passed ? 0 : 1);
        
    } else {
        // PARENT PROCESS (SERVER) 
        close(sv[0]);
        
        // Process the POST request 
        handle_client(sv[1]);
        
        close(sv[1]);
        
        // Wait for child and check if test passed
        int status;
        wait(&status);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

bool test_missing_file() {
    std::cout << "\n=== Test 3: Request Non-existent File ===\n";
    
    // Create connected sockets
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        return false;
    }
    
    // Split into client and server
    pid_t pid = fork();
    
    if (pid == 0) {
        // CHILD PROCESS (CLIENT) 
        close(sv[1]);
        
        // Request a file that doesn't exist
        const char* request = 
            "GET /chickebutt.html HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "\r\n";
        
        send(sv[0], request, strlen(request), 0);
        
        // Wait for response
        char buf[4096];
        ssize_t n = recv(sv[0], buf, sizeof(buf)-1, 0);
        
        bool passed = false;
        if (n > 0) {
            buf[n] = '\0';
            
            // Get "404 Not Found" for missing file
            if (strstr(buf, "404")) {
                std::cout << "[PASS] Received 404 Not Found\n";
                passed = true;
            } else {
                std::cout << "[FAIL] Expected 404 for missing file\n";
            }
        }
        
        close(sv[0]);
        exit(passed ? 0 : 1);
        
    } else {
        // PARENT PROCESS (SERVER)
        close(sv[0]);
        
        // Try to serve the non-existent file
        handle_client(sv[1]);
        
        close(sv[1]);
        
        // Check test result
        int status;
        wait(&status);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

bool test_malformed_request() {
    std::cout << "\n=== Test 4: Malformed Request ===\n";
    
    // Create connected sockets
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        return false;
    }
    
    // Split into client and server
    pid_t pid = fork();
    
    if (pid == 0) {
        // CHILD PROCESS (CLIENT)
        close(sv[1]);
        
        // Send data that's not valid HTTP
        // This tests if the server crashes on bad input
        const char* request = "NOT HTTP\r\n\r\n";
        send(sv[0], request, strlen(request), 0);
        
        // Try to read response 
        char buf[4096];
        recv(sv[0], buf, sizeof(buf)-1, 0);
        
        // As long as the server doesn't crash, we consider this a pass
        std::cout << "[PASS] Server handled malformed request without crashing\n";
        
        close(sv[0]);
        exit(0);  // Always pass if we got here
        
    } else {
        // PARENT PROCESS (SERVER)
        close(sv[0]);
        
        // Process the garbage request
        // If handle_client crashes, this whole process will crash
        // If it handles it gracefully, we pass
        handle_client(sv[1]);
        
        close(sv[1]);
        
        // Check if server survived
        int status;
        wait(&status);
        return true; // As long as we didn't crash, we pass
    }
}

int main() {
    std::cout << "=== Starting HTTP Parser Tests ===\n";
    std::cout << "Note: Tests expect www/ directory to exist for full validation\n";
    
    int passed = 0;
    int total = 4;
    
    if (test_valid_get_request()) passed++;
    if (test_invalid_method()) passed++;
    if (test_missing_file()) passed++;
    if (test_malformed_request()) passed++;
    
    std::cout << "\n=== Test Results: " << passed << "/" << total << " passed ===\n";
    
    if (passed == total) {
        std::cout << "[SUCCESS] All tests passed!\n";
        return 0;  
    } else {
        std::cout << "[FAILURE] Some tests failed.\n";
        return 1; 
    }
}