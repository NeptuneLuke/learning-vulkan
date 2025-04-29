#include "vk_core.hpp"

#include "util.hpp"
using namespace my_util;

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors
#include <string>
#include <cstring>      // strcmp()


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
	LOG_MESSAGE("Getting validation layers...", Color::White, Color::Black, 4);

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
	LOG_MESSAGE("Getting extensions...", Color::White, Color::Black, 4);

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
		throw std::runtime_error("Failed to create a Vulkan Instance!  \033[0m \n");
	}
	LOG_MESSAGE("Vulkan Instance created. \n", Color::Yellow, Color::Black, 0);
}


void select_physical_device(VkInstance instance, VkPhysicalDevice& physical_device) {

	LOG_MESSAGE("Selecting Physical Device...", Color::Yellow, Color::Black, 0);

	uint32_t devices_count = 0;
	vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);

	if (devices_count == 0) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("No GPU with Vulkan support found! \033[0m \n");
	}

	std::vector<VkPhysicalDevice> devices(devices_count);
	vkEnumeratePhysicalDevices(instance, &devices_count, devices.data());

	LOG_MESSAGE("Available Physical Devices: ", Color::White, Color::Black, 4);
	LOG_MESSAGE("Name | API | Driver", Color::White, Color::Black, 6);
	VkPhysicalDeviceProperties device_properties;
	for (const auto& dev : devices) {

		vkGetPhysicalDeviceProperties(dev, &device_properties);
		std::string dev_log = device_properties.deviceName;
		dev_log += " | " + std::to_string(device_properties.apiVersion) +
			       " | " + std::to_string(device_properties.driverVersion);
		LOG_MESSAGE(dev_log, Color::White, Color::Black, 6);

		if (check_device_suitable(dev)) {
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


bool check_device_suitable(VkPhysicalDevice physical_device) {

	QueueFamilyIndices indices = check_queue_families(physical_device);
	return indices.is_complete();
}


QueueFamilyIndices check_queue_families(VkPhysicalDevice physical_device) {

	LOG_MESSAGE("Querying Queue Families...", Color::White, Color::Black, 4);

	QueueFamilyIndices indices;

	uint32_t queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());

	// Request at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
	LOG_MESSAGE("Available Queue Families:", Color::White, Color::Black, 6);
	LOG_MESSAGE("Count | Flags | Index", Color::White, Color::Black, 8);
	int i = 0;
	for (const auto& qfam : queue_families) {

		std::string qfam_log = std::to_string(qfam.queueCount);
		qfam_log += " | " + std::to_string(qfam.queueFlags) +
			        " | " + std::to_string(i);
		LOG_MESSAGE(qfam_log, Color::White, Color::Black, 8);

		if (qfam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics_family = i;
		}

		if (indices.is_complete()) {
			break;
		}

		i++;
	}

	return indices;
}