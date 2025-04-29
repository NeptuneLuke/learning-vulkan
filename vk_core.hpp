#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>	// include GLFW definitions and load the Vulkan header

#include <vector>


/* -------------------- -------------------- */
const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

#ifdef _DEBUG
	const bool ENABLE_VALIDATION_LAYERS = true;
#else
	const bool ENABLE_VALIDATION_LAYERS = false;
#endif
/* -------------------- -------------------- */


// Check which validation layers are available
VkResult check_validation_layers_support();


// Initialize the Vulkan library
void create_vk_instance(VkInstance& instance);