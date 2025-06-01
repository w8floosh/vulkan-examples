CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

first-steps: main.cpp
	g++ $(CFLAGS) -o VulkanTest.out main.cpp $(LDFLAGS)

.PHONY: test clean

test: first-steps
	./VulkanTest.out

clean:
	rm -f VulkanTest.out