#pragma once
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

class Shader {

public:
	std::vector<char> vertexShaderSource;
	std::vector<char> fragmentShaderSource;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;

	VkShaderModule vertexShader;
	VkShaderModule fragmentShader;

	VkDevice device;

	Shader(const std::string ShaderName, VkDevice device);
	~Shader();
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(std::vector<char> code, VkDevice device, std::string ShaderName);

};