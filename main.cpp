#include "vk_core.hpp"

#include "util.hpp"
using namespace my_util;

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors
#include <cstdlib>		// miscellaneous utilities (EXIT_SUCCESS, EXIT_FAILURE)



/* -------------------- -------------------- */
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
/* -------------------- -------------------- */


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
	VkPhysicalDevice physical_device = VK_NULL_HANDLE; // implicitly destroyed
	VkDevice device;

	VkQueue queue_graphics; // implicitly destroyed

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

		create_vk_instance(instance);

		select_physical_device(physical_device, instance);

		create_logical_device(device, queue_graphics, physical_device, instance);
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

		LOG_MESSAGE("Destroying the Vulkan Logical Device...", Color::Bright_Blue, Color::Black, 0);
		vkDestroyDevice(device, nullptr);

		LOG_MESSAGE("Destroying the Vulkan Instance...", Color::Bright_Blue, Color::Black, 0);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate(); // Shutdown GLFW library
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