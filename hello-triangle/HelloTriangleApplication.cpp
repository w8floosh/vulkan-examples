#include "HelloTriangleApplication.h"
#include "DeviceUtils.h"
#include "FileUtils.h"
#include <set>

void HelloTriangleApplication::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window = glfwCreateWindow(WIDTH, HEIGHT, APP_NAME, nullptr, nullptr);
}

void HelloTriangleApplication::createInstance() {
  if (enableValidationLayers && !checkValidationLayerSupport())
    throw std::runtime_error("validation layers requested, but not available!");

  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = APP_NAME,
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  auto requiredExtensions = getRequiredExtensions();
  uint32_t availableExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount,
                                         nullptr);
  std::vector<VkExtensionProperties> availableExtensions(
      availableExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount,
                                         availableExtensions.data());
  std::cout << "Available extensions:" << std::endl;
  for (const auto &extension : availableExtensions)
    std::cout << '\t' << extension.extensionName << std::endl;
  if (requiredExtensions.size() > 0 &&
      !std::all_of(requiredExtensions.begin(), requiredExtensions.end(),
                   [&](const char *ext) {
                     return std::any_of(
                         availableExtensions.begin(), availableExtensions.end(),
                         [&](const VkExtensionProperties &availableExt) {
                           return strcmp(ext, availableExt.extensionName) == 0;
                         });
                   })) {
    throw std::runtime_error("Missing required extensions");
  }

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(
          enableValidationLayers ? validationLayers.size() : 0),
      .ppEnabledLayerNames =
          enableValidationLayers ? validationLayers.data() : nullptr,
      .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
      .ppEnabledExtensionNames = requiredExtensions.data(),
  };
  if (enableValidationLayers) {
    createInfo.pNext = &debugCreateInfo;
    populateDebugMessengerCreateInfo(debugCreateInfo);
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    throw std::runtime_error("failed to create instance!");
}

void HelloTriangleApplication::setupDebugMessenger() {
  if (!enableValidationLayers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(createInfo);

  if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS)
    throw std::runtime_error("failed to set up debug messenger!");
}

void HelloTriangleApplication::createSurface() {
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create window surface!");
}

void HelloTriangleApplication::selectPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0)
    throw std::runtime_error("failed to find GPUs with Vulkan support!");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // Use an ordered map to automatically sort candidates by increasing score
  std::multimap<int, VkPhysicalDevice> candidates;

  for (const auto &device : devices) {
    int score = rateDeviceSuitability(device, surface);
    candidates.insert(std::make_pair(score, device));
  }

  // Check if the best candidate is suitable at all
  if (candidates.rbegin()->first > 0) {
    physicalDevice = candidates.rbegin()->second;
  } else {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

  if (physicalDevice == VK_NULL_HANDLE)
    throw std::runtime_error("failed to find a suitable GPU!");
}

void HelloTriangleApplication::createLogicalDevice() {
  auto indices =
      findQueueFamilies(physicalDevice, surface, VK_QUEUE_GRAPHICS_BIT);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentationFamily.value()};
  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies)
    queueCreateInfos.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    });

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = static_cast<uint32_t>(
          enableValidationLayers ? validationLayers.size() : 0),
      .ppEnabledLayerNames =
          enableValidationLayers ? validationLayers.data() : nullptr,
      .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
      .ppEnabledExtensionNames = deviceExtensions.data(),
      .pEnabledFeatures = &deviceFeatures,
  };

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create logical device!");

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &qGraphics);
  vkGetDeviceQueue(device, indices.presentationFamily.value(), 0,
                   &qPresentation);
}

void HelloTriangleApplication::createSwapChain() {
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = nullptr,
      .surface = surface,
      .minImageCount = imageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1, // is more than 1 if the image is stereoscopic 3D
      .imageUsage =
          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // use
                                               // VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                               // if you need to render to a
                                               // separate image and then
                                               // postprocess on it
      .preTransform = swapChainSupport.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
  };

  QueueFamilyIndices indices =
      findQueueFamilies(physicalDevice, surface, VK_QUEUE_GRAPHICS_BIT);

  if (indices.graphicsFamily != indices.presentationFamily) {
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentationFamily.value()};
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount =
        static_cast<uint32_t>(std::size(queueFamilyIndices));
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create swap chain!");

  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());
  swapChainImageFormat = surfaceFormat.format;
  swapChainExtent = extent;
}

void HelloTriangleApplication::createImageViews() {
  swapChainImageViews.resize(swapChainImages.size());
  for (size_t i = 0; i < swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .image = swapChainImages[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapChainImageFormat,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    if (vkCreateImageView(device, &createInfo, nullptr,
                          &swapChainImageViews[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to create image views!");
  }
}

void HelloTriangleApplication::createRenderPass() {
  VkAttachmentDescription colorAttachment{
      .format = swapChainImageFormat,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  VkAttachmentReference colorAttachmentRef{
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  VkSubpassDescription subpass{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentRef,
  };

  VkSubpassDependency dependency{
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
  };

  VkRenderPassCreateInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &colorAttachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
  };

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create render pass!");
}

void HelloTriangleApplication::createGraphicsPipeline(bool useDynamicState) {
  auto vertShaderCode = readFile("shaders/vert.spv");
  auto fragShaderCode = readFile("shaders/frag.spv");
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertShaderModule,
      .pName = "main",
  };

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragShaderModule,
      .pName = "main",
  };

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkPipelineDynamicStateCreateInfo dynamicState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data(),
  };

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 0,
      .vertexAttributeDescriptionCount = 0,
  };

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE,
  };

  VkViewport viewport{
      .x = 0.0f,
      .y = 0.0f,
      .width = (float)swapChainExtent.width,
      .height = (float)swapChainExtent.height,
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  VkRect2D scissor{
      .offset = {0, 0},
      .extent = swapChainExtent,
  };

  VkPipelineViewportStateCreateInfo viewportState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = useDynamicState ? nullptr : &viewport,
      .scissorCount = 1,
      .pScissors = useDynamicState ? nullptr : &scissor,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .lineWidth = 1.0f};

  VkPipelineMultisampleStateCreateInfo multisampling{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
  };

  VkPipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo colorBlending{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment,
  };

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
  };

  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("failed to create pipeline layout!");

  VkGraphicsPipelineCreateInfo pipelineInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .stageCount = 2,
      .pStages = shaderStages,
      .pVertexInputState = &vertexInputInfo,
      .pInputAssemblyState = &inputAssembly,
      .pViewportState = &viewportState,
      .pRasterizationState = &rasterizer,
      .pMultisampleState = &multisampling,
      .pDepthStencilState = nullptr, // Optional
      .pColorBlendState = &colorBlending,
      .pDynamicState = useDynamicState ? &dynamicState : nullptr, // Optional
      .layout = pipelineLayout,
      .renderPass = renderPass,
      .basePipelineHandle = VK_NULL_HANDLE, // Optional
      .basePipelineIndex = -1,              // Optional
  };
  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &graphicsPipeline) != VK_SUCCESS)
    throw std::runtime_error("failed to create graphics pipeline!");

  vkDestroyShaderModule(device, fragShaderModule, nullptr);
  vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void HelloTriangleApplication::createFramebuffers() {
  swapChainFramebuffers.resize(swapChainImageViews.size());
  for (size_t i = 0; i < swapChainImageViews.size(); i++) {
    VkImageView attachments[] = {swapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = swapChainExtent.width,
        .height = swapChainExtent.height,
        .layers = 1,
    };

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                            &swapChainFramebuffers[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to create framebuffer!");
  }
}

void HelloTriangleApplication::createCommandPool() {
  QueueFamilyIndices queueFamilyIndices =
      findQueueFamilies(physicalDevice, surface, VK_QUEUE_GRAPHICS_BIT);

  VkCommandPoolCreateInfo poolInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
  };

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create command pool!");
}

void HelloTriangleApplication::createCommandBuffer() {
  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to allocate command buffer!");
}

void HelloTriangleApplication::recordCommandBuffer(
    VkCommandBuffer commandBuffer, uint32_t imageIndex) {

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pInheritanceInfo = nullptr,
  };

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    throw std::runtime_error("failed to begin recording command buffer!");

  VkClearValue clearColor{
      .color =
          {
              .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
          },
  };
  VkRenderPassBeginInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = renderPass,
      .framebuffer = swapChainFramebuffers[imageIndex],
      .renderArea =
          {
              .offset = {0, 0},
              .extent = swapChainExtent,
          },
      .clearValueCount = 1,
      .pClearValues = &clearColor};

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPipeline);

  VkViewport viewport{
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(swapChainExtent.width),
      .height = static_cast<float>(swapChainExtent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{
      .offset = {0, 0},
      .extent = swapChainExtent,
  };
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    throw std::runtime_error("failed to record command buffer!");
}

void HelloTriangleApplication::createSyncObjects() {
  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
  };

  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &imageAvailableSemaphore) != VK_SUCCESS)
    throw std::runtime_error("failed to create image available semaphore!");

  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &renderFinishedSemaphore) != VK_SUCCESS)
    throw std::runtime_error("failed to create render finished semaphore!");

  VkFenceCreateInfo fenceInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
    throw std::runtime_error("failed to create in flight fence!");
}

void HelloTriangleApplication::draw() {
  vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &inFlightFence);
  uint32_t imageIndex;
  // Add error handling
  vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore,
                        VK_NULL_HANDLE, &imageIndex);
  vkResetCommandBuffer(commandBuffer, 0);
  recordCommandBuffer(commandBuffer, imageIndex);

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
  };
  VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
  VkCommandBuffer commandBuffers[] = {commandBuffer};
  VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                          .pNext = nullptr,
                          .waitSemaphoreCount = 1,
                          .pWaitSemaphores = waitSemaphores,
                          .pWaitDstStageMask = waitStages,
                          .commandBufferCount = 1,
                          .pCommandBuffers = &commandBuffer,
                          .signalSemaphoreCount = 1,
                          .pSignalSemaphores = signalSemaphores};

  if (vkQueueSubmit(qGraphics, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    throw std::runtime_error("failed to submit draw command buffer!");

  VkSwapchainKHR swapChains[] = {swapChain};
  VkPresentInfoKHR presentInfo{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = signalSemaphores,
      .swapchainCount = 1,
      .pSwapchains = swapChains,
      .pImageIndices = &imageIndex,
      .pResults = nullptr, // Optional
  };

  // Add error handling
  vkQueuePresentKHR(qPresentation, &presentInfo);
}
