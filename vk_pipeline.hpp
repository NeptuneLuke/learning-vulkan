#pragma once

#include "vk_includes.hpp"


namespace vk_pipeline {


// Initialize the Graphics Pipeline
void create_pipeline(VkPipelineLayout pipeline_layout, VkDevice device);


// Create a shader module for each of the vertex and fragment shader
VkShaderModule create_shader_module(const std::vector<char>& shader_code, VkDevice device);

} // namespace vk_pipeline