#pragma once
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

class Shader {

public:
	std::vector<char> vertexShaderSource;
	std::vector<char> fragmentShaderSource;
	std::vector<char> tessellationControlShaderSource;
	std::vector<char> tessellationEvalShaderSource;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;

	VkShaderModule vertexShader;
	VkShaderModule fragmentShader;
	VkShaderModule tessellationControlShader;
	VkShaderModule tessellationEvalShader;

	VkDevice device;

	Shader(const std::string ShaderName, VkDevice device);
	~Shader();
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(std::vector<char> code, VkDevice device, std::string ShaderName);

};