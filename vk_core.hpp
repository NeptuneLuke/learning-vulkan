#pragma once

#include "vk_includes.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> // include Windows specific headers

#include <optional>     // has_value()


namespace vk_core {


struct QueueFamilyIndices {

	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	bool is_complete() {

		return graphics_family.has_value() &&
			   present_family.has_value();
	}
};

struct SwapchainSupportDetails {

	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};



// Initialize the Vulkan library
void create_vk_instance(VkInstance& instance);


// Initialize the Vulkan-Windows surface
void create_vk_surface(VkSurfaceKHR& surface, VkInstance instance, GLFWwindow* window);


// Select the Physical device (GPU)
void select_physical_device(VkPhysicalDevice& physical_device, VkInstance instance, VkSurfaceKHR surface);


// Initialize Logical Device
void create_logical_device(
	VkDevice& device, VkPhysicalDevice physical_device,
	VkInstance instance, VkSurfaceKHR surface,
	VkQueue& queue_graphics, VkQueue& queue_present);


// Initialize Swapchain
void create_swapchain(
	VkSwapchainKHR& swapchain, std::vector<VkImage>& swapchain_images,
	VkFormat swapchain_image_format, VkExtent2D swapchain_extent,
	VkSurfaceKHR surface, GLFWwindow* window,
	VkPhysicalDevice physical_device, VkDevice device);


// Query for specific swapchain features/details
SwapchainSupportDetails query_swapchain_support(VkSurfaceKHR surface, VkPhysicalDevice physical_device);


// Set the swapchain color space settings
VkSurfaceFormatKHR choose_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);


// Set the conditions for how to show/swap images to the screen
VkPresentModeKHR choose_swapchain_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);


// Set the resolution of the images in the swapchain
VkExtent2D choose_swapchain_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);


// Initialize Image Views
void create_image_views(
	std::vector<VkImageView>& swapchain_image_views,
	std::vector<VkImage> swapchain_images,
	VkFormat swapchain_image_format,
	VkDevice device);


// Check which validation layers are available
VkResult check_validation_layers_support();


// Check which extensions are required
std::vector<const char*> check_glfw_required_extensions();


// Check if the required extensions match with
// all the available extensions supported by the device
bool check_device_extension_support(VkPhysicalDevice physical_device);


// Check if the physical device is suitable
// for the operations we want to perform
bool check_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface);


// Check for queue families supported by the physical device
QueueFamilyIndices check_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface);


// Print physical devices features in detail
void print_physical_devices(VkPhysicalDevice dev);


} // namespace vk_core