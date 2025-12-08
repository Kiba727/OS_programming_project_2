CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -pthread

SRCS = main.cpp socket.cpp http_parser.cpp thread_pool.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = server

TEST_TARGETS = test_pool test_socket test_parser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

tests: $(TEST_TARGETS)

test_pool: test_thread_pool.o thread_pool.o
	$(CXX) $(CXXFLAGS) -o test_pool test_thread_pool.o thread_pool.o

test_socket: test_socket.o socket.o
	$(CXX) $(CXXFLAGS) -o test_socket test_socket.o socket.o

test_parser: test_parser.o http_parser.o
	$(CXX) $(CXXFLAGS) -o test_parser test_parser.o http_parser.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) $(TEST_TARGETS) *.o