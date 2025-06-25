# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11

# Raylib flags
RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Target executable
TARGET = 3d_cube

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp $(RAYLIB_FLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
