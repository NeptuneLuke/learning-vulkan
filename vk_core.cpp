#include "vk_core.hpp"

#include "util.hpp"
using namespace my_util;

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors
#include <string>
#include <cstring>      // strcmp()


// Initialize the Vulkan library
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
		throw std::runtime_error("Validation layers not available! \n");
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

	uint32_t extensions_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

	// Query extensions details
	#ifdef _DEBUG
		std::vector<VkExtensionProperties> extensions_detail(extensions_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, extensions_detail.data());

		LOG_MESSAGE("Available extensions : " + std::to_string(extensions_count), Color::White, Color::Black, 6);

		for (const auto& ext : extensions_detail) {
			std::string message = ext.extensionName;
			message += " | v." + std::to_string(ext.specVersion);
			LOG_MESSAGE(message, Color::White, Color::Black, 8);
		}
	#endif

	// Set extensions
	const char** extensions;
	extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

	instance_info.enabledExtensionCount = extensions_count;
	instance_info.ppEnabledExtensionNames = extensions;
	instance_info.enabledLayerCount = 0;


	if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create a Vulkan Instance! \n");
	}
	LOG_MESSAGE("Vulkan Instance created. \n", Color::Yellow, Color::Black, 0);
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