#pragma once

#define VK_USE_PLATFORM_WIN32_KHR // enables Vulkan-Windows specific implementations
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>	// include GLFW definitions and load the Vulkan header

#include <vector>


// Required validation layers
const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

// Required extensions
const std::vector<const char*> DEVICE_EXTENSIONS = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef _DEBUG
const bool ENABLE_VALIDATION_LAYERS = true;
#else
const bool ENABLE_VALIDATION_LAYERS = false;
#endif