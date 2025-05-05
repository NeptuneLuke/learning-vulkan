#include "vk_pipeline.hpp"
#include "my_util.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept> // std::runtime_error()


using namespace my_util; // my_util.hpp


namespace vk_pipeline {


void create_pipeline(VkPipelineLayout pipeline_layout, VkDevice device) {

	LOG_MESSAGE("Creating Vulkan Pipeline...", Color::Yellow, Color::Black, 0);
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

	LOG_MESSAGE("Shader modules attached to the pipeline", Color::Bright_White, Color::Black, 4);


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


	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;
	pipeline_layout_info.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
		std::cout << "\033[31;40m";
		throw std::runtime_error("Failed to create Vulkan Pipeline! \033[0m \n");
	}

	LOG_MESSAGE("Vulkan Pipeline created. \n", Color::Yellow, Color::Black, 0);

	vkDestroyShaderModule(device, frag_shader_module, nullptr);
	vkDestroyShaderModule(device, vert_shader_module, nullptr);
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

} // namespace vk_pipeline