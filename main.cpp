#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>	// include GLFW definitions and load the Vulkan header

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors
#include <cstdlib>		// miscellaneous utilities (EXIT_SUCCESS, EXIT_FAILURE)
#include <vector>


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;


/*
This class stores the Vulkan objects as private members
and interacts with them through functions.
*/
class HelloTriangle {

public:

	void run() {

		// If at any time during execution an error occurs,
		// we'll throw a std::runtime_error exception
		// which will propagate back to the main function and catched
		// by the general std::exception.

		init_window();

		init_vulkan();

		main_loop();

		cleanup();
	}


private:

	VkInstance instance;

	GLFWwindow* window;

	/* -------------------- -------------------- */
	// Initialize a GLFW window
	void init_window() {

		glfwInit();    // Initialize GLFW library

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not create an OpenGL context -> GLFW_NO_API
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	  // For now, disable window resizing

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	// Initialize the Vulkan objects
	void init_vulkan() {

		create_vk_instance();
	}


	// Iterates render operations until the window is closed
	void main_loop() {

		// Keep running until an error occurs or the window is closed
		while(!glfwWindowShouldClose(window)) {

			glfwPollEvents();
		}

	}

	// Deallocate resources
	void cleanup() {

		std::cout << "Destroying the Vulkan Instance... \n";
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate(); // Shutdown GLFW library
	}
	/* -------------------- -------------------- */


	/* -------------------- -------------------- */
	// Initialize the Vulkan library
	void create_vk_instance() {

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

		// Supported extensions
		uint32_t extensions_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);

		// Query extensions details
		std::vector<VkExtensionProperties> extensions_detail(extensions_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, extensions_detail.data());

		std::cout << "Available extensions: " << extensions_count << " \n";
		for (const auto& ext : extensions_detail) {
			std::cout << "\t" << ext.extensionName << " | ver. " << ext.specVersion << "\n";
		}

		// Set extensions
		const char** extensions;
		extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

		instance_info.enabledExtensionCount = extensions_count;
		instance_info.ppEnabledExtensionNames = extensions;
		instance_info.enabledLayerCount = 0; // For now, leave empty

		if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS) {

			throw std::runtime_error("Failed to create a Vulkan Instance! \n");
		}
	}
	/* -------------------- -------------------- */
};


int main() {

	HelloTriangle application;

	try {

		application.run();
	}
	catch (const std::exception& ex) {

		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}