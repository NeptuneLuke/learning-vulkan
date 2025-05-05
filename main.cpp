#include "vk_core.hpp"
#include "vk_pipeline.hpp"
#include "my_util.hpp"

#include <iostream>		// reporting errors
#include <stdexcept>	// reporting errors: std::runtime_error()
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
	std::vector<VkFramebuffer> swapchain_framebuffers;

	VkPipeline pipeline;
	VkPipelineLayout pipeline_layout;
	VkRenderPass render_pass;

	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;

	VkSemaphore semaphore_image_available;
	VkSemaphore semaphore_render_finished;
	VkFence fence_in_flight;
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

		vk_pipeline::create_renderpass(render_pass, device, swapchain_image_format);

		vk_pipeline::create_pipeline(pipeline, pipeline_layout, render_pass, device);

		vk_pipeline::create_framebuffers(swapchain_framebuffers,
			                             swapchain_image_views,
			                             swapchain_extent,
			                             device, render_pass);

		vk_pipeline::create_command_pool(command_pool, physical_device, device, surface);

		vk_pipeline::create_command_buffer(command_buffer, command_pool, device);

		vk_pipeline::create_sync_objects(semaphore_image_available, semaphore_render_finished,
			                             fence_in_flight, device);
	}


	// Iterates render operations until the window is closed
	void main_loop() {

		// Keep running until an error occurs or the window is closed
		while(!glfwWindowShouldClose(window)) {

			glfwPollEvents();

			draw_frame();
		}

		// All the operations in draw_frame() are asynchronous.
		// When we exit the loop in main_loop(), some operations may still be going on.
		// Cleaning up resources while that is happening should be avoided.
		// The solution is wait for the logical device to finish operations.
		vkDeviceWaitIdle(device);
	}


	// Render a single frame of a scene
	void draw_frame() {

		// Wait for the previous frame to finish
		vkWaitForFences(device, 1, &fence_in_flight, VK_TRUE, UINT64_MAX);
		vkResetFences(device, 1, &fence_in_flight);

		uint32_t image_index = 0;
		// Acquire an image from the swapchain
		vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
			                  semaphore_image_available, VK_NULL_HANDLE, &image_index);


		// Record command buffer which draws the scene onto that image
		vkResetCommandBuffer(command_buffer, /*VkCommandBufferResetFlagBits*/ 0);
		vk_pipeline::record_command_buffer(command_buffer, image_index,
			                               pipeline, render_pass,
			                               swapchain_framebuffers, swapchain_extent);


		// Submit the command buffer
		VkSubmitInfo submit_commandbuffer_info{};
		submit_commandbuffer_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore semaphores_wait[] = { semaphore_image_available };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_commandbuffer_info.waitSemaphoreCount = 1;
		submit_commandbuffer_info.pWaitSemaphores = semaphores_wait;
		submit_commandbuffer_info.pWaitDstStageMask = waitStages;

		submit_commandbuffer_info.commandBufferCount = 1;
		submit_commandbuffer_info.pCommandBuffers = &command_buffer;

		VkSemaphore semaphores_signal[] = { semaphore_render_finished };
		submit_commandbuffer_info.signalSemaphoreCount = 1;
		submit_commandbuffer_info.pSignalSemaphores = semaphores_signal;


		if (vkQueueSubmit(queue_graphics, 1, &submit_commandbuffer_info, fence_in_flight) != VK_SUCCESS) {
			std::cout << "\033[31;40m";
			throw std::runtime_error("Failed to submit draw Command Buffer! \033[0m \n");
		}


		// Present the swapchain image
		VkPresentInfoKHR present_info{};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = semaphores_signal;

		VkSwapchainKHR swapchains[] = { swapchain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swapchains;
		present_info.pImageIndices = &image_index;

		vkQueuePresentKHR(queue_present, &present_info);
	}


	// Deallocate resources in opposite order of creation
	void cleanup() {

		LOG_MESSAGE("Destroying Vulkan Semaphore(s) and Fence(s)...", Color::Bright_Blue, Color::Black, 0);
		vkDestroySemaphore(device, semaphore_image_available, nullptr);
		vkDestroySemaphore(device, semaphore_render_finished, nullptr);
		vkDestroyFence(device, fence_in_flight, nullptr);

		LOG_MESSAGE("Destroying Vulkan Command Pool...", Color::Bright_Blue, Color::Black, 0);
		vkDestroyCommandPool(device, command_pool, nullptr);

		LOG_MESSAGE("Destroying Vulkan Swapchain Framebuffers...", Color::Bright_Blue, Color::Black, 0);
		for (auto framebuffer : swapchain_framebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		LOG_MESSAGE("Destroying Vulkan Pipeline...", Color::Bright_Blue, Color::Black, 0);
		vkDestroyPipeline(device, pipeline, nullptr);

		LOG_MESSAGE("Destroying Vulkan Pipeline Layout...", Color::Bright_Blue, Color::Black, 4);
		vkDestroyPipelineLayout(device, pipeline_layout, nullptr);

		LOG_MESSAGE("Destroying Vulkan Render pass...", Color::Bright_Blue, Color::Black, 0);
		vkDestroyRenderPass(device, render_pass, nullptr);

		LOG_MESSAGE("Destroying Vulkan Image Views...", Color::Bright_Blue, Color::Black, 0);
		for (auto imgv : swapchain_image_views) {
			vkDestroyImageView(device, imgv, nullptr);
		}

		LOG_MESSAGE("Destroying Vulkan Swapchain...", Color::Bright_Blue, Color::Black, 0);
		vkDestroySwapchainKHR(device, swapchain, nullptr);

		LOG_MESSAGE("Destroying Vulkan Logical Device...", Color::Bright_Blue, Color::Black, 0);
		LOG_MESSAGE("Destroying Queues...", Color::Bright_Blue, Color::Black, 4);
		vkDestroyDevice(device, nullptr);

		LOG_MESSAGE("Destroying Vulkan-Windows Surface...", Color::Bright_Blue, Color::Black, 0);
		vkDestroySurfaceKHR(instance, surface, nullptr);

		LOG_MESSAGE("Destroying Vulkan Instance...", Color::Bright_Blue, Color::Black, 0);
		LOG_MESSAGE("Releasing Physical Device...", Color::Bright_Blue, Color::Black, 4);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate(); // Shutdown GLFW library
	}
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