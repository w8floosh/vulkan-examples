#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

void createBuffer(VkDevice logicalDevice, VkPhysicalDevice device,
                  VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &bufferMemory);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);