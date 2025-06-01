#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
#include <vector>

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

std::vector<const char *> getRequiredExtensions();
bool checkDeviceExtensionSupport(VkPhysicalDevice device);

// DEVICE INITIALIZATION

// Swap chain support details for a physical device.
// The swap chain is a series of images that are presented to the screen.
// The swap chain is created with a set of swap chain images, formats,
// present modes, and capabilities.
// The swap chain images are the images that are presented to the screen.
// The swap chain formats are the formats of the swap chain images.
// The swap chain present modes are the modes in which the swap chain
// images are presented to the screen.
// The swap chain capabilities are the capabilities of the swap chain,
// such as the minimum and maximum number of images in the swap chain,
// the minimum and maximum image extent, and the supported transform
// operations.
// The swap chain capabilities are used to create the swap chain.
// The swap chain formats and present modes are used to create the swap
// chain images.
// The swap chain support details are used to create the swap chain.
// The swap chain support details are obtained by querying the physical
// device for its swap chain support details.
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

// Rate the device based on its properties and features.
// The score is a simple heuristic that considers the device type and
// maximum image dimension.
// The higher the score, the more suitable the device is.
// The score is not a definitive measure of suitability, but it can help
// to select a device that is likely to be suitable for the application.
uint32_t rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats);

VkPresentModeKHR chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes);

VkExtent2D chooseSwapExtent(GLFWwindow *window,
                            const VkSurfaceCapabilitiesKHR &capabilities);

// QUEUE FAMILIES MANAGEMENT

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentationFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentationFamily.has_value();
  }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice,
                                     VkSurfaceKHR surface,
                                     VkQueueFlags queueFlags);