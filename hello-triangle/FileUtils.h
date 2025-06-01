#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

std::vector<char> readFile(const std::string &filename);

VkShaderModule createShaderModule(VkDevice device,
                                  const std::vector<char> &code);