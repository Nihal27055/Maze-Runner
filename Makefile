# Makefile for C++ Maze Runner
# Requires SFML and OpenGL libraries

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lsfml-window -lsfml-system -lGL -lGLU

TARGET = maze_runner
SOURCE = maze_runner.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

