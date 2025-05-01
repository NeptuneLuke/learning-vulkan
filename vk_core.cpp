#include "vk_core.hpp"
#include "my_util.hpp"

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors
#include <string>
#include <cstring>      // strcmp()
#include <set>

using namespace my_util;


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
		throw std::runtime_error("Validation layers not available!  \033[0m \n");
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


	// Extensions (required by GLFW for the Vulkan instance)
	LOG_MESSAGE("Getting extensions...", Color::Bright_White, Color::Black, 4);

	std::vector<const char*> extensions = check_required_extensions();

	uint32_t extensions_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

	// Query extensions details
	#ifdef _DEBUG
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

	VkWin32SurfaceCreateInfoKHR surface_info{};
	surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_info.hwnd = glfwGetWin32Window(window); // window handle
	surface_info.hinstance = GetModuleHandle(nullptr);

	if (vkCreateWin32SurfaceKHR(instance, &surface_info, nullptr, &surface) != VK_SUCCESS) {
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

	std::vector<VkPhysicalDevice> devices(devices_count);
	vkEnumeratePhysicalDevices(instance, &devices_count, devices.data());

	LOG_MESSAGE("Available Physical Devices: ", Color::Bright_White, Color::Black, 4);
	LOG_MESSAGE("Name \t\t\t | Type \t | Vulkan API \t | Driver \t | Memory", Color::White, Color::Black, 6);
	VkPhysicalDeviceProperties device_properties;
	for (const auto& dev : devices) {

		vkGetPhysicalDeviceProperties(dev, &device_properties);
		VkPhysicalDeviceMemoryProperties device_memory;
		vkGetPhysicalDeviceMemoryProperties(dev, &device_memory);

		std::string dev_log = device_properties.deviceName ;

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


void create_logical_device(VkDevice& device, VkPhysicalDevice physical_device, VkInstance instance, VkSurfaceKHR surface, VkQueue& queue_graphics, VkQueue& queue_present) {

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

	// Create device
	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = static_cast<uint32_t>(queue_info.size());
	device_info.pQueueCreateInfos = queue_info.data();
	device_info.pEnabledFeatures = &device_features;
	device_info.enabledExtensionCount = 0;

	// Setup validation layers to device
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


std::vector<const char*> check_required_extensions() {

	uint32_t extensions_count = 0;
	const char** glfw_extensions;

	glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
	std::vector<const char*> extensions(glfw_extensions, glfw_extensions + extensions_count);

	if (ENABLE_VALIDATION_LAYERS) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


bool check_device_suitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {

	QueueFamilyIndices indices = check_queue_families(physical_device, surface);
	return indices.is_complete();
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