#pragma once
#define GLFW_INCLUDE_VULKAN
#include "DebugUtils.h"
#include <GLFW/glfw3.h>
#include <vector>

inline const uint32_t WIDTH = 800;
inline const uint32_t HEIGHT = 600;
inline const char *APP_NAME = "Hello Triangle";

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkSurfaceKHR surface;
  VkQueue qGraphics;
  VkQueue qPresentation;
  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;
  VkSemaphore imageAvailableSemaphore;
  VkSemaphore renderFinishedSemaphore;
  VkFence inFlightFence;

  // Initialize the GLFW window.
  // GLFW is a library for creating windows and handling input.
  // It is used to create a window for the application and handle input
  // events.
  // The window is created with a specified width, height, and title.
  // The window is created with the GLFW_NO_API flag, which means that
  // GLFW will not create an OpenGL context.
  // This is because we will be using Vulkan instead of OpenGL.
  // The window is created with the GLFW_RESIZABLE flag, which means that
  // the window can be resized by the user.
  void initWindow();

  // Initialize Vulkan.
  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    selectPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
  }

  // Create a Vulkan instance.
  // A Vulkan instance is the connection between the application and the
  // Vulkan library.
  // It is the first thing that needs to be created in a Vulkan application.
  // The instance is used to create other Vulkan objects, such as devices,
  // command pools, and swap chains.
  // The instance is created with a set of application and instance
  // extensions and layers.
  // The application extensions and layers are used to provide additional
  // functionality and validation to the application.
  // The instance extensions and layers are used to provide additional
  // functionality and validation to the Vulkan library.
  void createInstance();

  // Set up the debug messenger to receive validation layer messages.
  // The debug messenger is a callback that is called when a validation layer
  // message is generated.
  void setupDebugMessenger();

  void createSurface();

  // Select a physical device that is suitable for the application.

  // A physical device is a GPU that supports Vulkan.
  // The application can use multiple physical devices, but for this
  // example, we will only use one.
  void selectPhysicalDevice();

  // Create a logical device that is used to communicate with the physical
  // device.
  // A logical device is a representation of a physical device that is used
  // to create and manage Vulkan objects, such as command pools, swap chains,
  // and pipelines.
  // The logical device is created with a set of device extensions and
  // features.
  // The device extensions and features are used to provide additional
  // functionality and validation to the logical device.
  void createLogicalDevice();

  // Create a swap chain that is used to present images to the screen.
  // A swap chain is a series of images that are used to present images to
  // the screen.
  // The swap chain is created with a set of swap chain images and
  // presentation modes.
  // The swap chain images are the images that are used to present images to
  // the screen.
  // The presentation modes are the modes that are used to present images to
  // the screen.
  void createSwapChain();

  // Create image views for the swap chain images.
  // An image view is a representation of an image that is used to access the
  // image data.
  // The image view is created with a set of image view formats and
  // components.
  // The image view formats and components are used to provide additional
  // functionality and validation to the image view.
  // The image view is used to access the image data in the swap chain
  // images.
  void createImageViews();

  // Create a render pass that is used to render images to the swap chain
  // images.
  // A render pass is a series of attachments that are used to render
  // images to the swap chain images.
  // The render pass is created with a set of attachment descriptions and
  // subpasses.
  // The attachment descriptions and subpasses are used to provide
  // additional functionality and validation to the render pass.
  void createRenderPass();

  // Create a graphics pipeline that is used to render images to the swap
  // chain images.
  // A graphics pipeline is a series of shaders and states that are used to
  // render images to the swap chain images.
  // The graphics pipeline is created with a set of shader stages and
  // pipeline states.
  // The shader stages and pipeline states are used to provide additional
  // functionality and validation to the graphics pipeline.
  // The graphics pipeline is fully defined by:
  // - The vertex input state, which describes the format of the vertex data
  // - The input assembly state, which describes how the vertex data is
  //   assembled into primitives
  // - The viewport state, which describes the size and position of the
  //   viewport
  // - The rasterization state, which describes how the primitives are
  //   rasterized
  // - The multisample state, which describes how the primitives are
  //   multisampled
  // - The depth and stencil state, which describes how the primitives are
  //   depth and stencil tested
  // - The color blend state, which describes how the primitives are blended
  // - The dynamic state, which describes how the pipeline state can be
  //   changed dynamically
  // The graphics pipeline is created with a set of pipeline layout and
  // render pass.
  // The pipeline layout is used to describe the layout of the pipeline
  // resources, such as descriptor sets and push constants.
  // The render pass is used to describe the attachments that are used to
  // render images to the swap chain images.
  void createGraphicsPipeline(bool useDynamicState = false);

  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffer();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void createSyncObjects();
  // Main loop of the application.
  // The main loop is where the application does its work.
  // In this example, the main loop does nothing, but in a real application,
  // the main loop would handle input events, update the application state,
  // and render the application.
  // The main loop runs until the window is closed.
  // The main loop is implemented using a while loop that checks if the
  // window should close.
  // The main loop calls the glfwPollEvents function to process input events.
  // The glfwPollEvents function processes all pending events and calls the
  // appropriate callback functions.
  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      draw();
    }

    vkDeviceWaitIdle(device); // Wait for the device to finish all operations
  }

  void draw();
  // Clean up the application.
  // The cleanup function is called when the application is closed.
  // The cleanup function destroys the Vulkan instance and the GLFW window.
  // The cleanup function is implemented using the vkDestroyInstance and
  // glfwDestroyWindow functions.
  // The vkDestroyInstance function destroys the Vulkan instance.
  // The glfwDestroyWindow function destroys the GLFW window.
  // The glfwTerminate function terminates and cleans up the GLFW library.
  void cleanup() {
    if (enableValidationLayers)
      destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    for (auto framebuffer : swapChainFramebuffers) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (auto imageView : swapChainImageViews)
      vkDestroyImageView(device, imageView, nullptr);
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
  }
};