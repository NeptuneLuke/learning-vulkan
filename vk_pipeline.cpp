#include "vk_pipeline.hpp"
#include "my_util.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept> // std::runtime_error()


using namespace my_util; // my_util.hpp


namespace vk_pipeline {


void create_pipeline(VkDevice device) {

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


	LOG_MESSAGE("Destroying the Fragment shader...", Color::Bright_Blue, Color::Black, 0);
	vkDestroyShaderModule(device, frag_shader_module, nullptr);
	LOG_MESSAGE("Destroying the Vertex shader...", Color::Bright_Blue, Color::Black, 0);
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