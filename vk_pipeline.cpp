#include "vk_pipeline.hpp"
#include "vk_core.hpp"
#include "my_util.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept> // std::runtime_error()


using namespace my_util; // my_util.hpp


namespace vk_pipeline {


void create_pipeline(VkPipeline& pipeline, VkPipelineLayout& pipeline_layout,
	                 VkRenderPass render_pass, VkDevice device) {

	LOG_MESSAGE("Creating Vulkan Pipeline...", Color::Yellow, Color::Black, 0);

	// Creating Shader modules
	LOG_MESSAGE("Creating Shader modules...", Color::Bright_White, Color::Black, 4);

	auto vert_shader = read_file("shaders/vert.spv");
	auto frag_shader = read_file("shaders/frag.spv");

	VkShaderModule vert_shader_module = create_shader_module(vert_shader, device);
	VkShaderModule frag_shader_module = create_shader_module(frag_shader, device);

	VkPipelineShaderStageCreateInfo vert_shader_info{};
	vert_shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_info.module = vert_shader_module;
	vert_shader_info.pName = "main";

	VkPipelineShaderStageCreateInfo frag_shader_info{};
	frag_shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_info.module = frag_shader_module;
	frag_shader_info.pName = "main";

	VkPipelineShaderStageCreateInfo shader_stages[] = {
		vert_shader_info, frag_shader_info };

	LOG_MESSAGE("Shader modules attached to the pipeline.", Color::Bright_White, Color::Black, 4);


	// Setting up Pipeline features
	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 0;
	vertex_input_info.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo input_assembly{};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewport_state_info{};
	viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_info.viewportCount = 1;
	viewport_state_info.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer_info{};
	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = VK_FALSE;
	rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
	rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL; // fragments fill the area of polygons
	rasterizer_info.lineWidth = 1.0f;
	rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer_info.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling_info{};
	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable = VK_FALSE;
	multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState color_blend_attachment{};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		                                    VK_COLOR_COMPONENT_G_BIT |
		                                    VK_COLOR_COMPONENT_B_BIT |
		                                    VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo color_blending_info{};
	color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending_info.logicOpEnable = VK_FALSE;
	color_blending_info.logicOp = VK_LOGIC_OP_COPY;
	color_blending_info.attachmentCount = 1;
	color_blending_info.pAttachments = &color_blend_attachment;
	color_blending_info.blendConstants[0] = 0.0f;
	color_blending_info.blendConstants[1] = 0.0f;
	color_blending_info.blendConstants[2] = 0.0f;
	color_blending_info.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	dynamic_state_info.pDynamicStates = dynamic_states.data();


	// Creating Pipeline layout
	LOG_MESSAGE("Creating Vulkan Pipeline layout...", Color::Bright_White, Color::Black, 4);

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;
	pipeline_layout_info.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan Pipeline! \033[0m \n");
	}
	LOG_MESSAGE("Vulkan Pipeline layout created.", Color::Bright_White, Color::Black, 4);


	// Creating Pipeline

	VkGraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stages;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_state_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pColorBlendState = &color_blending_info;
	pipeline_info.pDynamicState = &dynamic_state_info;
	pipeline_info.layout = pipeline_layout;
	pipeline_info.renderPass = render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan Pipeline! \033[0m \n");
	}
	LOG_MESSAGE("Vulkan Pipeline created. \n", Color::Yellow, Color::Black, 0);

	vkDestroyShaderModule(device, frag_shader_module, nullptr);
	vkDestroyShaderModule(device, vert_shader_module, nullptr);
}


void create_renderpass(VkRenderPass& render_pass, VkDevice device, VkFormat swapchain_image_format) {

	LOG_MESSAGE("Creating Vulkan Render pass...", Color::Yellow, Color::Black, 0);

	// Single color buffer attachment as one of the images from the swapchain
	VkAttachmentDescription color_attachment{};
	color_attachment.format = swapchain_image_format; // format of color attachment should match with format of swapchain images
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

	// This refers to color and depth data
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;   // what to do before rendering (clear the framebuffer to black before drawing a new frame)
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // what to do after rendering (render the triangle on the screen, so store it)

	// This refers to stencil data.
	// We don't use stencil buffer, so we do not care.
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // we don't care what previous layout the image was in
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // the image must be ready for presentation using the swapchain after rendering


	VkAttachmentReference color_attachment_ref{};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;


	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan Render pass! \033[0m \n");
	}

	LOG_MESSAGE("Vulkan Render pass created. \n", Color::Yellow, Color::Black, 0);
}


VkShaderModule create_shader_module(const std::vector<char>& shader_code, VkDevice device) {

	VkShaderModuleCreateInfo shader_info{};
	shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_info.codeSize = shader_code.size();
	shader_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

	VkShaderModule shader_module;
	if (vkCreateShaderModule(device, &shader_info, nullptr, &shader_module) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Shader module! \033[0m \n");
	}

	return shader_module;
}


void create_framebuffers(std::vector<VkFramebuffer>& swapchain_framebuffers,
	                     std::vector<VkImageView> swapchain_image_views,
	                     VkExtent2D swapchain_extent,
	                     VkDevice device, VkRenderPass render_pass) {

	LOG_MESSAGE("Creating Vulkan Framebuffers...", Color::Yellow, Color::Black, 0);

	swapchain_framebuffers.resize(swapchain_image_views.size());

	for (size_t i=0; i < swapchain_image_views.size(); i++) {

		VkImageView attachments[] = { swapchain_image_views[i]};

		VkFramebufferCreateInfo framebuffer_info{};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = swapchain_extent.width;
		framebuffer_info.height = swapchain_extent.height;
		framebuffer_info.layers = 1;

		if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &swapchain_framebuffers[i]) != VK_SUCCESS) {
			std::cout << "\033[31;40m";
			throw std::runtime_error("Failed to create Vulkan Framebuffer! \033[0m \n");
		}
	}

	LOG_MESSAGE("Vulkan Framebuffers created. \n", Color::Yellow, Color::Black, 0);
}


void create_command_pool(VkCommandPool& command_pool,
	                     VkPhysicalDevice physical_device, VkDevice device,
	                     VkSurfaceKHR surface) {

	LOG_MESSAGE("Creating Vulkan Command Pool...", Color::Yellow, Color::Black, 0);

	vk_core::QueueFamilyIndices queue_family_indices =
		vk_core::check_queue_families(physical_device, surface);

	VkCommandPoolCreateInfo command_pool_info{};
	command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

	if (vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan Command Pool! \033[0m \n");
	}

	LOG_MESSAGE("Vulkan Command Pool created. \n", Color::Yellow, Color::Black, 0);
}


void create_command_buffer(VkCommandBuffer& command_buffer, VkCommandPool command_pool,
	                       VkDevice device) {

	LOG_MESSAGE("Creating Vulkan Command buffer(s)...", Color::Yellow, Color::Black, 0);

	VkCommandBufferAllocateInfo command_buffer_info{};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.commandPool = command_pool;
	command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device, &command_buffer_info, &command_buffer) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to allocate Command buffer(s)! \033[0m \n");
	}

	LOG_MESSAGE("Vulkan Command buffer(s) created. \n", Color::Yellow, Color::Black, 0);
}


void record_command_buffer(VkCommandBuffer command_buffer, uint32_t swapchain_image_index,
	                       VkPipeline pipeline, VkRenderPass render_pass,
	                       std::vector<VkFramebuffer> swapchain_framebuffers,
	                       VkExtent2D swapchain_extent) {

	LOG_MESSAGE("Registering Command buffer(s)...", Color::Yellow, Color::Black, 0);

	VkCommandBufferBeginInfo command_buffer_info{};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(command_buffer, &command_buffer_info) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to begin recording Command Buffer! \033[0m \n");
	}

	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = render_pass;
	render_pass_info.framebuffer = swapchain_framebuffers[swapchain_image_index];
	render_pass_info.renderArea.offset = { 0, 0 };
	render_pass_info.renderArea.extent = swapchain_extent;

	// The color that clears the screen after rendering
	VkClearValue clear_color = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clear_color;

	vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchain_extent.width;
	viewport.height = (float)swapchain_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain_extent;
	vkCmdSetScissor(command_buffer, 0, 1, &scissor);


	// Draw command of the triangle
	// 3 is the number of vertices in the vertex buffer, that we are not actually using now,
	// but we specify it anyway
	// 1, 0, 0 are just default values
	vkCmdDraw(command_buffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(command_buffer);


	if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to record Command Buffer! \033[0m \n");
	}

	LOG_MESSAGE("Command buffer(s) registered. \n", Color::Yellow, Color::Black, 0);
}

} // namespace vk_pipeline