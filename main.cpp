#include "vk_core.hpp"
#include "my_util.hpp"

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors
#include <cstdlib>		// miscellaneous utilities (EXIT_SUCCESS, EXIT_FAILURE)


using namespace my_util; // my_util.hpp


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

	GLFWwindow* window;

	VkInstance instance;
	VkSurfaceKHR surface;

	VkPhysicalDevice physical_device = VK_NULL_HANDLE; // implicitly destroyed in vkDestroyInstance()
	VkDevice device;

	// All queues are implicitly destoyed in vkDestroyDevice()
	VkQueue queue_graphics;
	VkQueue queue_present;

	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchain_images; // implicitly destroyed in vkDestroySwapchainKHR()
	VkFormat swapchain_image_format;
	VkExtent2D swapchain_extent;
	std::vector<VkImageView> swapchain_image_views;


	/* -------------------- -------------------- */
	// Initialize a GLFW window
	void init_window() {

		glfwInit();    // Initialize GLFW library

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not create an OpenGL context -> GLFW_NO_API
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	  // For now, disable window resizing

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan tutorial", nullptr, nullptr);
	}

	// Initialize the Vulkan objects
	void init_vulkan() {

		vk_core::create_vk_instance(instance);

		vk_core::create_vk_surface(surface, instance, window);

		vk_core::select_physical_device(physical_device, instance, surface);

		vk_core::create_logical_device(device, physical_device,
			                           instance, surface,
			                           queue_graphics, queue_present);

		vk_core::create_swapchain(swapchain, swapchain_images,
			                      swapchain_image_format, swapchain_extent,
			                      surface, window, physical_device, device);

		vk_core::create_image_views(swapchain_image_views,
			                        swapchain_images, swapchain_image_format,
			                        device);
	}


	// Iterates render operations until the window is closed
	void main_loop() {

		// Keep running until an error occurs or the window is closed
		while(!glfwWindowShouldClose(window)) {

			glfwPollEvents();
		}

	}

	// Deallocate resources in opposite order of creation
	void cleanup() {

		LOG_MESSAGE("Destroying the Vulkan Image Views...", Color::Bright_Blue, Color::Black, 0);
		for (auto imgv : swapchain_image_views) {
			vkDestroyImageView(device, imgv, nullptr);
		}

		LOG_MESSAGE("Destroying the Vulkan Swapchain...", Color::Bright_Blue, Color::Black, 0);
		vkDestroySwapchainKHR(device, swapchain, nullptr);

		LOG_MESSAGE("Destroying the Vulkan Logical Device...", Color::Bright_Blue, Color::Black, 0);
		LOG_MESSAGE("Destroying Queues...", Color::Bright_Blue, Color::Black, 4);
		vkDestroyDevice(device, nullptr);

		LOG_MESSAGE("Destroying the Vulkan-Windows Surface...", Color::Bright_Blue, Color::Black, 0);
		vkDestroySurfaceKHR(instance, surface, nullptr);

		LOG_MESSAGE("Destroying the Vulkan Instance...", Color::Bright_Blue, Color::Black, 0);
		LOG_MESSAGE("Destroying the Vulkan Physical Device...", Color::Bright_Blue, Color::Black, 4);
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