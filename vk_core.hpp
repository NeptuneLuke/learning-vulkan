#pragma once

#define VK_USE_PLATFORM_WIN32_KHR // enables Vulkan-Windows specific implementations
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>	// include GLFW definitions and load the Vulkan header

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> // include Windows specific headers

#include <vector>
#include <optional>     // has_value()


/* -------------------- -------------------- */
struct QueueFamilyIndices {

	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	bool is_complete() {

		return graphics_family.has_value() &&
			   present_family.has_value();
	}
};


const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

#ifdef _DEBUG
	const bool ENABLE_VALIDATION_LAYERS = true;
#else
	const bool ENABLE_VALIDATION_LAYERS = false;
#endif
/* -------------------- -------------------- */


// Initialize the Vulkan library
void create_vk_instance(VkInstance& instance);


// Initialize the Vulkan-Windows surface
void create_vk_surface(VkSurfaceKHR& surface, VkInstance instance, GLFWwindow* window);


// Select the Physical device (GPU)
void select_physical_device(VkPhysicalDevice& physical_device, VkInstance instance, VkSurfaceKHR surface);


// Initialize Logical Device
void create_logical_device(VkDevice& device, VkPhysicalDevice physical_device, VkInstance instance, VkSurfaceKHR surface, VkQueue& queue_graphics, VkQueue& queue_present);


// Check which validation layers are available
VkResult check_validation_layers_support();


// Check which extensions are required
std::vector<const char*> check_required_extensions();


// Check if the physical device is suitable
// for the operations we want to perform
bool check_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface);


// Check for queue families supporte by the
// physical device
QueueFamilyIndices check_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface);