#pragma once

#include "vk_includes.hpp"


namespace vk_pipeline {


// Initialize the Graphics Pipeline
void create_pipeline(
	VkPipeline& pipeline, VkPipelineLayout& pipeline_layout,
	VkRenderPass render_pass, VkDevice device);


// Initialize the Renderpass
void create_renderpass(VkRenderPass& render_pass, VkDevice device, VkFormat swapchain_image_format);


// Create a shader module for each of the vertex and fragment shader
VkShaderModule create_shader_module(const std::vector<char>& shader_code, VkDevice device);


// Initialize Swapchain Framebuffers
void create_framebuffers(
	std::vector<VkFramebuffer>& swapchain_framebuffers,
	std::vector<VkImageView> swapchain_image_views,
	VkExtent2D swapchain_extent,
	VkDevice device, VkRenderPass render_pass);


// Initialize Command Pool
void create_command_pool(
	VkCommandPool& command_pool,
	VkPhysicalDevice physical_device, VkDevice device,
	VkSurfaceKHR surface);


// Initialize Command buffer
void create_command_buffer(
	VkCommandBuffer& command_buffer, VkCommandPool command_pool,
	VkDevice device);


// Write commands
void record_command_buffer(
	VkCommandBuffer command_buffer, uint32_t swapchain_image_index,
	VkPipeline pipeline, VkRenderPass render_pass,
	std::vector<VkFramebuffer> swapchain_framebuffers,
	VkExtent2D swapchain_extent);

} // namespace vk_pipeline