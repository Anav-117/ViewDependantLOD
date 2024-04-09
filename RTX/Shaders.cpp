#include "Shaders.h"
#include <iostream>

Shader::Shader(const std::string ShaderName, VkDevice device) {

    vertexShaderSource = readFile("./Shaders/" + ShaderName + "_vert.spv");
    fragmentShaderSource = readFile("./Shaders/" + ShaderName + "_frag.spv");
    tessellationControlShaderSource = readFile("./Shaders/" + ShaderName + "_tesc.spv");
    tessellationEvalShaderSource = readFile("./Shaders/" + ShaderName + "_tese.spv");

    vertexShader = createShaderModule(vertexShaderSource, device, ShaderName);
    fragmentShader = createShaderModule(fragmentShaderSource, device, ShaderName);
    tessellationControlShader = createShaderModule(tessellationControlShaderSource, device, ShaderName);
    tessellationEvalShader = createShaderModule(tessellationEvalShaderSource, device, ShaderName);

    this->device = device;

    VkPipelineShaderStageCreateInfo vshaderInfo{};
    vshaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vshaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vshaderInfo.module = vertexShader;
    vshaderInfo.pName = "main";

    shaderStageInfos.push_back(vshaderInfo);
    
    VkPipelineShaderStageCreateInfo fshaderInfo{};
    fshaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fshaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fshaderInfo.module = fragmentShader;
    fshaderInfo.pName = "main";

    shaderStageInfos.push_back(fshaderInfo);

    VkPipelineShaderStageCreateInfo tcshaderInfo{};
    tcshaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    tcshaderInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    tcshaderInfo.module = tessellationControlShader;
    tcshaderInfo.pName = "main";

    shaderStageInfos.push_back(tcshaderInfo);

    VkPipelineShaderStageCreateInfo teshaderInfo{};
    teshaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    teshaderInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    teshaderInfo.module = tessellationEvalShader;
    teshaderInfo.pName = "main";

    shaderStageInfos.push_back(teshaderInfo);

}

Shader::~Shader() {

    vkDestroyShaderModule(device, vertexShader, nullptr);
    vkDestroyShaderModule(device, fragmentShader, nullptr);
    vkDestroyShaderModule(device, tessellationControlShader, nullptr);
    vkDestroyShaderModule(device, tessellationEvalShader, nullptr);
    
}

std::vector<char> Shader::readFile(const std::string& filename) {

    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;

}

VkShaderModule Shader::createShaderModule(std::vector<char> code, VkDevice device, std::string ShaderName) {

    VkShaderModule shader;

    VkShaderModuleCreateInfo shaderInfo{};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = code.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device, &shaderInfo, nullptr, &shader) != VK_SUCCESS) {
        throw std::runtime_error("Failed To Create Shader" + ShaderName + "\n");
    }

    return shader;

}
