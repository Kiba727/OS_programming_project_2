CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -pthread

# Define three separate server executables
TARGETS = server_single server_multi server_pool

TEST_TARGETS = test_pool test_socket test_parser

# Common objects used by all servers
COMMON_OBJS = socket.o http_parser.o

all: $(TARGETS)

# 1. Single Threaded Server
server_single: server_singlethread.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o server_single server_singlethread.o $(COMMON_OBJS)

# 2. Multi Threaded Server
server_multi: server_multithread.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o server_multi server_multithread.o $(COMMON_OBJS)

# 3. Thread Pool Server (Needs thread_pool.o as well)
server_pool: server_threadpool.o $(COMMON_OBJS) thread_pool.o
	$(CXX) $(CXXFLAGS) -o server_pool server_threadpool.o $(COMMON_OBJS) thread_pool.o

# Tests
tests: $(TEST_TARGETS)

test_pool: test_thread_pool.o thread_pool.o
	$(CXX) $(CXXFLAGS) -o test_pool test_thread_pool.o thread_pool.o

test_socket: test_socket.o socket.o
	$(CXX) $(CXXFLAGS) -o test_socket test_socket.o socket.o

test_parser: test_parser.o http_parser.o
	$(CXX) $(CXXFLAGS) -o test_parser test_parser.o http_parser.o

# Generic rule to compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(TEST_TARGETS) *.o