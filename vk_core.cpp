#include "vk_core.hpp"
#include "my_util.hpp"

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors: std::runtime_error()
#include <string>
#include <cstring>      // strcmp()
#include <set>
#include <algorithm>    // clamp()
#include <limits>


using namespace my_util; // my_util.hpp


namespace vk_core {

void create_vk_instance(VkInstance& instance) {

	LOG_MESSAGE("Creating Vulkan Instance...", Color::Yellow, Color::Black, 0);

	// Informations about the application (optional but useful)
	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Hello Triangle";
	app_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.apiVersion = VK_API_VERSION_1_4; // Vulkan API version


	/*
	Specify which global extensions and validation layers to use.
	Vulkan is platform agnostic, so it needs an extension to interface
	with the window system.
	Vulkan uses validation layers as debugging and additional operations support,
	and to avoid relying on different drivers/systems behaviour.
	*/
	VkInstanceCreateInfo instance_info{};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo = &app_info;


	// Validation Layers
	LOG_MESSAGE("Getting validation layers...", Color::Bright_White, Color::Black, 4);

	if (ENABLE_VALIDATION_LAYERS && check_validation_layers_support() != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Validation layers not available! \033[0m \n");
	}

	if (ENABLE_VALIDATION_LAYERS) {
		instance_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		instance_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();

	#ifdef _DEBUG
		LOG_MESSAGE("Available validation layers: " + std::to_string(VALIDATION_LAYERS.size()), Color::White, Color::Black, 6);
		for (const auto& layer : VALIDATION_LAYERS) {
			LOG_MESSAGE(layer, Color::White, Color::Black, 8);
		}
	#endif
	}
	else {
		instance_info.enabledLayerCount = 0;
	}


	// Extensions are required by GLFW.
	// In particular, VK_KHR_surface and VK_KHR_win32_surface are required.
	LOG_MESSAGE("Getting extensions...", Color::Bright_White, Color::Black, 4);

	std::vector<const char*> extensions = check_glfw_required_extensions();
	uint32_t extensions_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

	// Query extensions details
	#ifdef _DEBUG

		// Get all available extensions for our system
		std::vector<VkExtensionProperties> extensions_detail(extensions_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, extensions_detail.data());

		LOG_MESSAGE("Available extensions : " + std::to_string(extensions_count), Color::White, Color::Black, 6);

		for (const auto& ext : extensions_detail) {

			std::string ext_log = ext.extensionName;
			ext_log += " | v." + std::to_string(ext.specVersion);
			LOG_MESSAGE(ext_log, Color::White, Color::Black, 8);
		}
	#endif

	// Set extensions
	instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instance_info.ppEnabledExtensionNames = extensions.data();
	instance_info.enabledLayerCount = 0;


	if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan Instance! \033[0m \n");
	}
	LOG_MESSAGE("Vulkan Instance created. \n", Color::Yellow, Color::Black, 0);
}


void create_vk_surface(VkSurfaceKHR& surface, VkInstance instance, GLFWwindow* window) {

	LOG_MESSAGE("Creating Vulkan-Windows Surface...", Color::Yellow, Color::Black, 0);

	VkWin32SurfaceCreateInfoKHR win32_surface_info{};
	win32_surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	win32_surface_info.hwnd = glfwGetWin32Window(window); // window handle
	win32_surface_info.hinstance = GetModuleHandle(nullptr);

	if (vkCreateWin32SurfaceKHR(instance, &win32_surface_info, nullptr, &surface) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan-Windows Surface! \033[0m \n");
	}

	LOG_MESSAGE("Vulkan-Windows Surface created. \n", Color::Yellow, Color::Black, 0);

	// Or use the built-in GLFW function:
	// glfwCreateWindowSurface(instance, window, nullptr, &surface)
}


void select_physical_device(VkPhysicalDevice& physical_device, VkInstance instance, VkSurfaceKHR surface) {

	LOG_MESSAGE("Selecting Physical Device...", Color::Yellow, Color::Black, 0);

	uint32_t devices_count = 0;
	vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);

	if (devices_count == 0) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("No GPU with Vulkan support found! \033[0m \n");
	}

	// Get all physical devices
	std::vector<VkPhysicalDevice> devices(devices_count);
	vkEnumeratePhysicalDevices(instance, &devices_count, devices.data());

	for (const auto& dev : devices) {

		print_physical_devices(dev);

		if (check_device_suitable(dev, surface)) {
			physical_device = dev;
			break;
		}
	}

	if (physical_device == VK_NULL_HANDLE) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("No suitable GPU found! \033[0m \n");
	}

	LOG_MESSAGE("Physical Device selected. \n", Color::Yellow, Color::Black, 0);
}


void create_logical_device(VkDevice& device, VkPhysicalDevice physical_device,
	                       VkInstance instance, VkSurfaceKHR surface,
	                       VkQueue& queue_graphics, VkQueue& queue_present) {

	LOG_MESSAGE("Creating Vulkan Logical Device...", Color::Yellow, Color::Black, 0);

	// Specify the queues to be created
	QueueFamilyIndices indices = check_queue_families(physical_device, surface);

	std::vector<VkDeviceQueueCreateInfo> queue_info;

	// Create a set of all unique queue families necessary for
	// the required queues (graphics and presentation)
	std::set<uint32_t> unique_queue_families = {
		indices.graphics_family.value(),
	    indices.present_family.value()};

	float queue_priority = 1.0f;
	for (uint32_t qfam : unique_queue_families) {

		VkDeviceQueueCreateInfo queue{};
		queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue.queueFamilyIndex = qfam;
		queue.queueCount = 1;
		queue.pQueuePriorities = &queue_priority;

		queue_info.push_back(queue);
	}

	// Specify device features, previously queried with vkGetPhysicalDeviceFeatures()
	// For now we don't need anything special, so we simply define it.
	VkPhysicalDeviceFeatures device_features{};

	// Create logical device
	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = static_cast<uint32_t>(queue_info.size());
	device_info.pQueueCreateInfos = queue_info.data();
	device_info.pEnabledFeatures = &device_features;

	// Setup required extensions
	device_info.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
	device_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

	// Setup validation layers
	if (ENABLE_VALIDATION_LAYERS) {
		device_info.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		device_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	}
	else {
		device_info.enabledLayerCount = 0;
	}


	if (vkCreateDevice(physical_device, &device_info, nullptr, &device) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan Logical Device! \033[0m \n");
	}

	// Setup queues supported by the device
	vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &queue_graphics);
	vkGetDeviceQueue(device, indices.present_family.value(), 0, &queue_present);

	LOG_MESSAGE("Vulkan Logical Device created. \n", Color::Yellow, Color::Black, 0);
}


void create_swapchain(VkSwapchainKHR& swapchain, std::vector<VkImage> swapchain_images,
					  VkFormat swapchain_image_format, VkExtent2D swapchain_extent,
	                  VkSurfaceKHR surface, GLFWwindow* window,
	                  VkPhysicalDevice physical_device, VkDevice device) {

	LOG_MESSAGE("Creating Vulkan Swapchain...", Color::Yellow, Color::Black, 0);

	SwapchainSupportDetails swapchain_support = query_swapchain_support(surface, physical_device);

	VkSurfaceFormatKHR surface_format = choose_swapchain_surface_format(swapchain_support.formats);
	VkPresentModeKHR present_mode = choose_swapchain_present_mode(swapchain_support.present_modes);
	VkExtent2D extent = choose_swapchain_extent(window, swapchain_support.capabilities);

	// Also set how many images we want to the swapchain. Not required.
	// Set to at least one more image than the minimum to avoid waiting on the driver
	// to complete internal operations befoe we can acquire another image to render to.
	uint32_t images_count = swapchain_support.capabilities.minImageCount + 1;

	// Also do not exceed the maximum number supported.
	if (swapchain_support.capabilities.maxImageCount > 0
		&& images_count > swapchain_support.capabilities.maxImageCount) {

		images_count = swapchain_support.capabilities.maxImageCount;
	}

	// Setup the swapchain
	VkSwapchainCreateInfoKHR swapchain_info{};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.surface = surface;
	swapchain_info.minImageCount = images_count;
	swapchain_info.imageFormat = surface_format.format;
	swapchain_info.imageColorSpace = surface_format.colorSpace;
	swapchain_info.imageExtent = extent;
	swapchain_info.imageArrayLayers = 1; // amount of layers of each image (basically always 1)
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // images are used as color attachment because we render them directly

	// Specify that the swapchain images will be used across multiple queue families.
	// We will be drawing the images in the swapchain from the graphics queue and
	// then submitting them to the presentation queue.
	QueueFamilyIndices indices = check_queue_families(physical_device, surface);
	uint32_t queue_family_indices[] = {
		indices.graphics_family.value(),
		indices.present_family.value() };

	if (indices.graphics_family != indices.present_family) {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_info.queueFamilyIndexCount = 2;
		swapchain_info.pQueueFamilyIndices = queue_family_indices;
	}
	else {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_info.queueFamilyIndexCount = 0; // optional
		swapchain_info.pQueueFamilyIndices = nullptr; // optional
	}

	// We can specify that a transform should be applied to images in the swapchain
	// if it is supported : swapchain_support.capabilities.supportedTransform
	// We will not use this feature, so specify the current transform.
	swapchain_info.preTransform = swapchain_support.capabilities.currentTransform;

	// Specify if the alpha channel should be used for blending with other windows
	// in the window system. Basically always ignore it, so specify VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR.
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	swapchain_info.presentMode = present_mode;
	swapchain_info.clipped = VK_TRUE; // enables clipping
	swapchain_info.oldSwapchain = VK_NULL_HANDLE; // For now, we assume we will only create 1 swapchain


	if (vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan Swapchain! \033[0m \n");
	}


	vkGetSwapchainImagesKHR(device, swapchain, &images_count, nullptr);
	swapchain_images.resize(images_count);
	vkGetSwapchainImagesKHR(device, swapchain, &images_count, swapchain_images.data());

	swapchain_image_format = surface_format.format;
	swapchain_extent = extent;

	LOG_MESSAGE("Vulkan Swapchain created. \n", Color::Yellow, Color::Black, 0);
}


SwapchainSupportDetails query_swapchain_support(VkSurfaceKHR surface, VkPhysicalDevice physical_device) {

	SwapchainSupportDetails details;

	// Set supported capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);


	// Query supported surface formats
	uint32_t formats_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formats_count, nullptr);

	if (formats_count != 0) {
		details.formats.resize(formats_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formats_count, details.formats.data());
	}

	// Query supported presentation modes
	uint32_t present_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &present_count, nullptr);

	if (present_count != 0) {
		details.present_modes.resize(present_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_count, details.present_modes.data());
	}

	return details;
}


VkSurfaceFormatKHR choose_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) {

	for (const auto& form : available_formats) {

		// Format is 8 bits B,G,R,A channels
		// Color space is SRGB
		if (form.format == VK_FORMAT_B8G8R8A8_SRGB && form.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return form;
		}
	}

	// If the format VK_FORMAT_B8G8R8A8_SRGB, SRGB is not found,
	// return the first format retrieved
	return available_formats[0];
}


VkPresentModeKHR choose_swapchain_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) {

	for (const auto& pmode : available_present_modes) {

		// A variation of VK_PRESENT_MODE_FIFO_KHR.
		// Instead of blocking the application when the queue is full, the images that are already queued
		// are simply replaced with the newer ones. This mode can be used to render frames as fast as possible.
		// It is also known as triple buffering.
		if (pmode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return pmode;
		}
	}

	// The swapchain is a queue where the display takes an image from the front of the queue
	// when the display is refreshed and the program inserts rendered images at the back of the queue.
	// If the queue is full then the program has to wait. This is most similar to vertical sync as found in modern games.
	// The moment that the display is refreshed is known as "vertical blank".
	return VK_PRESENT_MODE_FIFO_KHR;
}


VkExtent2D choose_swapchain_extent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {

	// The swap extent is the resolution of the swapchain images and
	// it's almost always exactly equal to the resolution of the window
	// that we're drawing to in pixels.

	// UINT32_MAX == std::numeric_limits<uint32_t>::max()
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D extent = { static_cast<uint32_t>(width),
		                      static_cast<uint32_t>(height) };

		// Bound the values of width and height between the min and max extents
		// supported by the swapchain
		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return extent;
	}
}


void create_image_views(std::vector<VkImageView> swapchain_image_views,
	                    std::vector<VkImage> swapchain_images,
	                    VkFormat swapchain_image_format,
	                    VkDevice device) {

	LOG_MESSAGE("Creating Vulkan Image Views...", Color::Yellow, Color::Black, 0);

	swapchain_image_views.resize(swapchain_images.size());

	for (size_t i = 0; i < swapchain_images.size(); i++) {

		VkImageViewCreateInfo image_view_info{};
		image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_info.image = swapchain_images[i];

		// The type of the image
		image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // images will be 2D textures
		image_view_info.format = swapchain_image_format;

		// Set color channels
		image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// Image purpose
		image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // used as color targets
		image_view_info.subresourceRange.baseMipLevel = 0; // no mipmapping
		image_view_info.subresourceRange.levelCount = 1;
		image_view_info.subresourceRange.baseArrayLayer = 0;
		image_view_info.subresourceRange.layerCount = 1; // no multiple layers


		if (vkCreateImageView(device, &image_view_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan Image Views! \033[0m \n");
		}
	}

	LOG_MESSAGE("Vulkan Image Views created. \n", Color::Yellow, Color::Black, 0);
}


VkResult check_validation_layers_support() {

	uint32_t layers_count = 0;
	vkEnumerateInstanceLayerProperties(&layers_count, nullptr);

	std::vector<VkLayerProperties> validation_layers(layers_count);
	vkEnumerateInstanceLayerProperties(&layers_count, validation_layers.data());

	for (const char* layer_name : VALIDATION_LAYERS) {

		bool found = false;

		for (const auto& layer_properties : validation_layers) {
			if (strcmp(layer_name, layer_properties.layerName) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			return VK_ERROR_LAYER_NOT_PRESENT;
		}
	}

	return VK_SUCCESS;
}


std::vector<const char*> check_glfw_required_extensions() {

	uint32_t extensions_count = 0;
	const char** glfw_extensions;

	glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
	std::vector<const char*> extensions(glfw_extensions, glfw_extensions + extensions_count);

	LOG_MESSAGE("GLFW Required extensions:", Color::White, Color::Black, 6);
	for (const auto& ext : extensions) {
		LOG_MESSAGE(ext, Color::White, Color::Black, 8);
	}

	if (ENABLE_VALIDATION_LAYERS) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Debug messenger extension
	}

	return extensions;
}


bool check_device_extension_support(VkPhysicalDevice physical_device) {

	uint32_t extensions_count;
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr);

	// Get all the available device extensions
	std::vector<VkExtensionProperties> extensions(extensions_count);
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, extensions.data());

	// Get only required extensions
	std::set<std::string> required_extensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

	// Overlap required with device supported extensions
	for (const auto& ext : extensions) {
		required_extensions.erase(ext.extensionName);
	}

	return required_extensions.empty();
}


bool check_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

	QueueFamilyIndices indices = check_queue_families(physical_device, surface);

	bool extensions_supported = check_device_extension_support(physical_device);

	// Swapchain support is sufficient if there is at least one supported
	// image format and one supported presentation mode for the surface provided
	bool swapchain_adequate = false;
	if (extensions_supported) {
		SwapchainSupportDetails swapchain_support = query_swapchain_support(surface, physical_device);
		swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty();
	}

	// Only query for swapchain support after veryfying that the swapchain extension
	// ( VK_KHR_SWAPCHAIN_EXTENSION_NAME ) is available
	return indices.is_complete() && extensions_supported && swapchain_adequate;
}


QueueFamilyIndices check_queue_families(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

	LOG_MESSAGE("Querying Queue Families...", Color::Bright_White, Color::Black, 4);

	QueueFamilyIndices indices;

	uint32_t queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());

	// Request at least one queue family that supports
	// VK_QUEUE_GRAPHICS_BIT(graphics queue) and presentation queue
	LOG_MESSAGE("Available Queue Families:", Color::White, Color::Black, 6);
	LOG_MESSAGE("Type \t\t | Count | Flags | Index", Color::White, Color::Black, 8);

	int i = 0;
	for (const auto& qfam : queue_families) {

		std::string qfam_log = std::to_string(qfam.queueCount);
		qfam_log += " \t | " + std::to_string(qfam.queueFlags) +
			        " \t | " + std::to_string(i);

		if (qfam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics_family = i;

			LOG_MESSAGE("Graphics \t | " + qfam_log, Color::White, Color::Black, 8);
		}

		VkBool32 present_family_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_family_support);
		if (present_family_support) {
			indices.present_family = i;

			LOG_MESSAGE("Presentation \t | " + qfam_log, Color::White, Color::Black, 8);
		}

		if (indices.is_complete()) {
			break;
		}

		i++;
	}

	return indices;
}


void print_physical_devices(VkPhysicalDevice dev) {

	LOG_MESSAGE("Available Physical Devices: ", Color::Bright_White, Color::Black, 4);
	LOG_MESSAGE("Name \t\t\t | Type \t | Vulkan API \t | Driver \t | Memory", Color::White, Color::Black, 6);

	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(dev, &device_properties);
	VkPhysicalDeviceMemoryProperties device_memory;
	vkGetPhysicalDeviceMemoryProperties(dev, &device_memory);

	std::string dev_log = device_properties.deviceName;

	std::string type = " | Unknown";
	switch (device_properties.deviceType) {

		default:

		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			type = "Unknown";
			break;

		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			type = "Integrated";
			break;

		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			type = "Discrete";
			break;

		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			type = "Virtual";
			break;

		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			type = "CPU";
			break;
	}

	float memory_gib = 0.0f;
	for (uint32_t i = 0; i < device_memory.memoryHeapCount; i++) {

		memory_gib = ((static_cast<float>(device_memory.memoryHeaps[i].size)) / 1024.0f / 1024.0f / 1024.0f);
	}

	dev_log += " \t | "
		+ type
		+ " \t | "
		+ std::to_string(VK_VERSION_MAJOR(device_properties.apiVersion)) + "."
		+ std::to_string(VK_VERSION_MINOR(device_properties.apiVersion)) + "."
		+ std::to_string(VK_VERSION_PATCH(device_properties.apiVersion))
		+ " \t | "
		+ std::to_string(VK_VERSION_MAJOR(device_properties.driverVersion)) + "."
		+ std::to_string(VK_VERSION_MINOR(device_properties.driverVersion)) + "."
		+ std::to_string(VK_VERSION_PATCH(device_properties.driverVersion))
		+ " \t | "
		+ std::to_string(memory_gib)
		+ " GiB";

	LOG_MESSAGE(dev_log, Color::White, Color::Black, 6);
}

} // namespace vk_core