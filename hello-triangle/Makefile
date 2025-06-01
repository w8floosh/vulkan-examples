CXX = g++
CXXFLAGS = -std=c++17 -g -O0 -Wall -Wextra 
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Find all .cpp files
SOURCES := $(wildcard *.cpp)
# Generate corresponding .o files
OBJECTS := $(SOURCES:.cpp=.o)
# Name of the executable
TARGET = HelloTriangle.out

# Default target
all: $(TARGET)

# Link all object files into the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .cpp into .o + track .h dependencies
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# If some .cpp don't have a corresponding .h, handle them separately
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)