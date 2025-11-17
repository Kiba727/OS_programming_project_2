CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

SRCS = main.cpp socket.cpp http_parser.cpp thread_pool.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
