#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string.h>
#include <vector>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

/*
  Check if the requested validation layers are available.
  vkEnumerateInstanceLayerProperties returns the number of available
  validation layers and their properties.
  If the requested validation layers are not available, return false.
*/
bool checkValidationLayerSupport();

/*
  Populate the VkDebugUtilsMessengerCreateInfoEXT struct with the
  required information.
  The createInfo struct is used to create the debug messenger.
  The debug messenger is used to receive validation layer messages.
*/
void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);

/*
  VkDebugUtilsMessageSeverityFlagBitsEXT bitmask specifies the severity of the
  message.
    - VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
    - VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like
  the creation of a resource
    - VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    Message about behavior that is not necessarily an error, but very likely a
  bug in your application
    - VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message
    about behavior that is invalid and may cause crashes

  VkDebugUtilsMessageTypeFlagsEXT bitmask specifies the type of the message.
    - VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that
  is unrelated to the specification or performance
    - VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened
  that violates the specification or indicates a possible mistake
    - VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential non-optimal use
  of Vulkan

  VkDebugUtilsMessengerCallbackDataEXT struct contains the message and other
  information about the callback.
    - pMessage: The debug message as a null-terminated string
    - pObjects: Array of Vulkan object handles related to the message
    - objectCount: Number of objects in array
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

/*
    Create a debug messenger to receive validation layer messages.
    The debug messenger is created with the createInfo struct.
    The debug messenger is used to receive validation layer messages.
*/
VkResult createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

/*
    Destroy the debug messenger.
    The debug messenger is destroyed with the destroyInfo struct.
    The debug messenger is used to receive validation layer messages.
*/
void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator);
