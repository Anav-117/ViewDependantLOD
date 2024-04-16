#include "VkConfig.h"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//std::vector<Vertex> vertices = {
//	{glm::vec3(0.5, -0.5, -0.0)},
//	{glm::vec3(-0.5, 0.5, -0.0)},
//	{glm::vec3(0.5, 0.5,  -0.0)},
//
//	{glm::vec3(-0.5, 0.5, -0.0)},
//	{glm::vec3(0.5, -0.5, -0.0)},
//	{glm::vec3(-0.5, -0.5, -0.0)},
//
//	//Back
//	{glm::vec3(-0.5, 0.5, 0.5)},
//	{glm::vec3(-0.5, -0.5, 0.5)},
//	{glm::vec3(0.5, 0.5,  0.5)},
//
//	{glm::vec3(0.5, 0.5, 0.5)},
//	{glm::vec3(-0.5, -0.5, 0.5)},
//	{glm::vec3(0.5, -0.5, 0.5)},
//
//	//Top
//	{glm::vec3(0.5, 0.5, 0.5)},
//	{glm::vec3(-0.5, 0.5, 0.0)},
//	{glm::vec3(-0.5, 0.5, 0.5)},
//
//	{glm::vec3(0.5, 0.5, 0.5)},
//	{glm::vec3(0.5, 0.5, 0.0)},
//	{glm::vec3(-0.5, 0.5, 0.0)},
//
//	//bottom
//	{glm::vec3(0.5, -0.5, 0.5)},
//	{glm::vec3(-0.5, -0.5, 0.5)},
//	{glm::vec3(-0.5, -0.5, 0.0)},
//
//	{glm::vec3(0.5, -0.5, 0.5)},
//	{glm::vec3(-0.5, -0.5, 0.0)},
//	{glm::vec3(0.5, -0.5, 0.0)},
//
//	//Right
//	{glm::vec3(0.5, 0.5, 0.0)},
//	{glm::vec3(0.5, 0.5, 0.5)},
//	{glm::vec3(0.5, -0.5, 0.0)},
//
//	{glm::vec3(0.5, 0.5, 0.5)},
//	{glm::vec3(0.5, -0.5, 0.5)},
//	{glm::vec3(0.5, -0.5, 0.0)},
//
//	//Left
//	{glm::vec3(-0.5, 0.5, 0.0)},
//	{glm::vec3(-0.5, -0.5, 0.5)},
//	{glm::vec3(-0.5, 0.5, 0.5)},
//
//	{glm::vec3(-0.5, 0.5, 0.0)},
//	{glm::vec3(-0.5, -0.5, -0.0)},
//	{glm::vec3(-0.5, -0.5, 0.5)}
//};

std::vector<const char*> VulkanClass::getRequiredExtensions() {

	uint32_t glfwExtentionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtentionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;

}

bool VulkanClass::checkValidationLayerSupport() {

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;

}

VulkanClass::VulkanClass() {

}

VulkanClass::VulkanClass(GLFWwindow* win) {

	window = win;
	createInstance();

	createSurface();

	physicalDevice = findPhysicalDevice();
	createLogicalDevice();

	createSwapChain();
	createImageViews();

	loadModel();

	createRenderPass();
	createDescriptorSetLayout();
	createDescriptorPools();
	createGraphicsPipeline();

	createFramebuffers();

	createCommandPool();
	createCommandBuffer();

	createVertexBuffer();
	//createIndexBuffer();

	createSyncObjects();

	//initImGui();

}

VulkanClass::~VulkanClass() {

	for (size_t i = 0; i < swapChain.framebuffers.size(); i++) {
		vkDestroyFramebuffer(logicalDevice, swapChain.framebuffers[i], nullptr);
	}
	for (size_t i = 0; i < swapChain.imageViews.size(); i++) {
		vkDestroyImageView(logicalDevice, swapChain.imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(logicalDevice, swapChain.__swapChain, nullptr);

	vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
	vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);

	//vkDestroyDescriptorPool(logicalDevice, imguiDescriptorPool, nullptr);
	//ImGui_ImplVulkan_Shutdown();

	for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(logicalDevice, transformBuffer[i], nullptr);
		vkFreeMemory(logicalDevice, transformBufferMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(logicalDevice, uniformDescriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(logicalDevice, transformDescriptorSetLayout, nullptr);

	vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	for (size_t i=0; i<swapChain.MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSempahore[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFence[i], nullptr);
	}

	vkDestroyFence(logicalDevice, imGuiFence, nullptr);

	for (size_t i=0; i<swapChain.MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer[i]);
	}
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	vkDestroyDevice(logicalDevice, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);

	vkDestroyInstance(instance, nullptr);

}

void VulkanClass::createInstance() {

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation Layers Requested But Not Found\n");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Lego Ocean";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	appInfo.pNext = nullptr;

	auto extensions = getRequiredExtensions();

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	} 

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan Instance\n");
	}

}

bool VulkanClass::findQueueFamilies(VkPhysicalDevice device) {

	uint32_t physicalDeviceQueueFamilyCount;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &physicalDeviceQueueFamilyCount, nullptr);
	
	if (physicalDeviceQueueFamilyCount == 0) {
		throw std::runtime_error("Cannot Find Any Queue Families On Physical Device\n");
	}

	std::vector<VkQueueFamilyProperties> queueFamilies(physicalDeviceQueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &physicalDeviceQueueFamilyCount, queueFamilies.data());

	uint32_t i = 0;
	for (auto queueFamily : queueFamilies) {
		if (queueFamily.queueFlags | VK_QUEUE_GRAPHICS_BIT > 0) {
			QueueFamilyIndex.graphicsFamily = i;
			return true;
		}
		
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			QueueFamilyIndex.presentFamily = i;
		}

		i++;
	}

	return false;

}

bool VulkanClass::checkSwapChainSupport(VkPhysicalDevice device) {

	SwapChainSupport details{};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t numFormats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, nullptr);
	details.formats.resize(numFormats);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, details.formats.data());

	uint32_t numPresentModes;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numPresentModes, nullptr);
	details.presentModes.resize(numPresentModes);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numPresentModes, details.presentModes.data());

	if (!details.formats.empty() && !details.presentModes.empty()) {
		SwapChainDetails = details;
		return true;
	}

	return false;

}

VkPhysicalDevice VulkanClass::findPhysicalDevice() {

	VkPhysicalDevice selectedDevice = NULL;

	uint32_t numSupportedDevices;

	if (vkEnumeratePhysicalDevices(instance, &numSupportedDevices, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("No Supported Physical Devices Found\n");
	}

	std::vector<VkPhysicalDevice> physicalDevices(numSupportedDevices);

	vkEnumeratePhysicalDevices(instance, &numSupportedDevices, physicalDevices.data());

	for (auto device : physicalDevices) {
		
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(device, &features);

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		if (!findQueueFamilies(device) || !requiredExtensions.empty() || properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || !(features.tessellationShader) || !checkSwapChainSupport(device)) {
			continue;
		}

		selectedDevice = device;
		std::cout << properties.vendorID << " | " << properties.deviceName << " | " << properties.deviceType << " | " << properties.driverVersion << "\n";

	}

	if (selectedDevice == NULL) {
		throw std::runtime_error("Cannot Find Suitable Physical Device\n");
	}

	return selectedDevice;

}

void VulkanClass::createLogicalDevice() {

	float queuePriority = 1.0;
	std::vector<VkDeviceQueueCreateInfo> queueInfos;

	std::set<uint32_t> UniqueQueueFamilies = {QueueFamilyIndex.graphicsFamily, QueueFamilyIndex.presentFamily};

	for (auto queue : UniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.pQueuePriorities = &queuePriority;
		queueInfo.queueCount = 1;
		queueInfo.queueFamilyIndex = queue;
		queueInfo.flags = 0;
		queueInfo.pNext = nullptr;
		queueInfos.push_back(queueInfo);
	}

	VkPhysicalDeviceFeatures supportedFeatures{};
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

	VkPhysicalDeviceFeatures requiredFeatures{};
	requiredFeatures.tessellationShader = VK_TRUE;
	requiredFeatures.fillModeNonSolid = VK_TRUE;
	requiredFeatures.wideLines = VK_TRUE;

	VkDeviceCreateInfo logicalDeviceCreateInfo{};

	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.pNext = nullptr;
	logicalDeviceCreateInfo.flags = 0;
	logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	logicalDeviceCreateInfo.pQueueCreateInfos = queueInfos.data();
	logicalDeviceCreateInfo.pEnabledFeatures = &requiredFeatures;
	if (enableValidationLayers) {
		logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		logicalDeviceCreateInfo.enabledLayerCount = 0;
		logicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;
	}
	logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	logicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Create Logical Device\n");
	}

	vkGetDeviceQueue(logicalDevice, QueueFamilyIndex.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, QueueFamilyIndex.presentFamily, 0, &presentQueue);

}

VkSurfaceFormatKHR SwapChain::findSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

	for (const auto format : availableFormats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	return availableFormats[0];

}

VkPresentModeKHR SwapChain::findSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

	for (const auto& presentMode : availablePresentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;

}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

}

void VulkanClass::createSwapChain() {

	swapChain.format = swapChain.findSwapChainFormat(SwapChainDetails.formats);
	swapChain.presentMode = swapChain.findSwapChainPresentMode(SwapChainDetails.presentModes);
	swapChain.extent = swapChain.chooseSwapExtent(SwapChainDetails.capabilities, window);

	uint32_t imageCount = SwapChainDetails.capabilities.minImageCount + 1;

	if (SwapChainDetails.capabilities.maxImageCount > 0 && imageCount > SwapChainDetails.capabilities.maxImageCount) {
		imageCount = SwapChainDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapChainInfo{};
	swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.surface = surface;
	swapChainInfo.minImageCount = imageCount;
	swapChainInfo.imageFormat = swapChain.format.format;
	swapChainInfo.imageColorSpace = swapChain.format.colorSpace;
	swapChainInfo.presentMode = swapChain.presentMode;
	swapChainInfo.imageExtent = swapChain.extent;
	swapChainInfo.imageArrayLayers = 1;
	swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	uint32_t queueFamilyIndices[] = { QueueFamilyIndex.graphicsFamily, QueueFamilyIndex.presentFamily };
	if (QueueFamilyIndex.graphicsFamily != QueueFamilyIndex.presentFamily) {
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainInfo.queueFamilyIndexCount = 2;
		swapChainInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapChainInfo.preTransform = SwapChainDetails.capabilities.currentTransform;
	swapChainInfo.clipped = VK_TRUE;
	swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicalDevice, &swapChainInfo, nullptr, &swapChain.__swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Create Swapchain\n");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain.__swapChain, &imageCount, nullptr);
	swapChain.images.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain.__swapChain, &imageCount, swapChain.images.data());

}

void VulkanClass::createSurface() {

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Create Window Surface\n");
	}

}

void VulkanClass::createImageViews() {

	swapChain.imageViews.resize(swapChain.images.size());

	for (size_t i = 0; i < swapChain.imageViews.size(); i++) {
		VkImageViewCreateInfo imageViewInfo{};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image = swapChain.images[i];
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = swapChain.format.format;
		imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;

		if (vkCreateImageView(logicalDevice, &imageViewInfo, nullptr, &swapChain.imageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed To Create Image View\n");
		}
	}

}

void VulkanClass::createRenderPass() {

	VkAttachmentDescription attachmentInfo{};
	attachmentInfo.format = swapChain.format.format;
	attachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentRef{};
	attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentRef.attachment = 0;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachmentRef;

	VkSubpassDependency dependencies{};
	dependencies.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies.dstSubpass = 0;
	dependencies.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies.srcAccessMask = 0;
	dependencies.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &attachmentInfo;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependencies;

	if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Create Render Pass\n");
	}

}

void VulkanClass::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding transformLayoutBinding{};
	transformLayoutBinding.binding = 0;
	transformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	transformLayoutBinding.descriptorCount = 1;
	transformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

	VkDescriptorSetLayoutCreateInfo transformLayoutInfo{};
	transformLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	transformLayoutInfo.bindingCount = 1;
	transformLayoutInfo.pBindings = &transformLayoutBinding;

	if (vkCreateDescriptorSetLayout(logicalDevice, &transformLayoutInfo, nullptr, &transformDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Transform Descriptor Set layout\n");
	}
}

void VulkanClass::createDescriptorPools() {

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(swapChain.MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(swapChain.MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &uniformDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Uniform Descriptor Pool\n");
	}

}

uint32_t VulkanClass::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");

}

void VulkanClass::createTransformBuffer(VkDeviceSize bufferSize) {

	transformBuffer.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
	transformBufferMemory.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
	transformBufferMap.resize(swapChain.MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &transformBuffer[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed To create Transform Uniform Buffer\n");

		VkMemoryRequirements memreq;
		vkGetBufferMemoryRequirements(logicalDevice, transformBuffer[i], &memreq);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memreq.size;
		allocInfo.memoryTypeIndex = findMemoryType(memreq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &transformBufferMemory[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to Allocate Transform Uniform Buffer Memory\n6");

		vkBindBufferMemory(logicalDevice, transformBuffer[i], transformBufferMemory[i], 0);

		vkMapMemory(logicalDevice, transformBufferMemory[i], 0, bufferSize, 0, &transformBufferMap[i]);
	}

}

void VulkanClass::createTransformDescriptorSet() {

	std::vector<VkDescriptorSetLayout> layouts(static_cast<uint32_t>(swapChain.MAX_FRAMES_IN_FLIGHT), transformDescriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = uniformDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	transformDescriptorSet.resize(static_cast<uint32_t>(swapChain.MAX_FRAMES_IN_FLIGHT));

	if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, transformDescriptorSet.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Transform Descriptor Set\n");
	}

	for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = transformBuffer[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(transform);

		VkWriteDescriptorSet transformWrite{};
		transformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		transformWrite.dstSet = transformDescriptorSet[i];
		transformWrite.dstBinding = 0;
		transformWrite.dstArrayElement = 0;
		transformWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		transformWrite.descriptorCount = 1;
		transformWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(logicalDevice, 1, &transformWrite, 0, nullptr);
	}

}

void VulkanClass::updateTransform() {

	for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) {
		memcpy(transformBufferMap[i], &transform, sizeof(transform));
	}

}


void VulkanClass::createGraphicsPipeline() {

	Shader basicShader("shader", logicalDevice);

	VkVertexInputBindingDescription vertexBindingInfo{};
	vertexBindingInfo.binding = 0;
	vertexBindingInfo.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexBindingInfo.stride = sizeof(Vertex);

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = 1;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.pVertexBindingDescriptions = &vertexBindingInfo;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChain.extent.width);
	viewport.height = static_cast<float>(swapChain.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissorRect{};
	scissorRect.extent = swapChain.extent;
	scissorRect.offset = { 0,0 };

	VkPipelineViewportStateCreateInfo viewportStateInfo{};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.scissorCount = 1;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pScissors = &scissorRect;
	viewportStateInfo.pViewports = &viewport;

	VkPipelineRasterizationStateCreateInfo rasterInfo{};
	rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterInfo.depthClampEnable = VK_FALSE;
	rasterInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterInfo.polygonMode = VK_POLYGON_MODE_LINE;
	rasterInfo.lineWidth = 3.0f;
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterInfo.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampleInfo{};
	multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo.sampleShadingEnable = VK_FALSE;
	multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlend{};
	colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlend.blendEnable = VK_FALSE;
	VkPipelineColorBlendStateCreateInfo colorBlendGlobal{};
	colorBlendGlobal.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendGlobal.logicOpEnable = VK_FALSE;
	colorBlendGlobal.attachmentCount = 1;
	colorBlendGlobal.pAttachments = &colorBlend;

	VkPipelineTessellationStateCreateInfo tessellationInfo{};
	tessellationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tessellationInfo.patchControlPoints = 3;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &transformDescriptorSetLayout;

	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Create Pipeline Layout\n");
	}


	//CREATING GRAPHICS PIPELINE

	VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
	graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineInfo.stageCount = basicShader.shaderStageInfos.size();
	graphicsPipelineInfo.pStages = basicShader.shaderStageInfos.data();
	graphicsPipelineInfo.pTessellationState = &tessellationInfo;
	graphicsPipelineInfo.pColorBlendState = &colorBlendGlobal;
	graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
	graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	graphicsPipelineInfo.pMultisampleState = &multisampleInfo;
	graphicsPipelineInfo.pRasterizationState = &rasterInfo;
	graphicsPipelineInfo.pViewportState = &viewportStateInfo;
	graphicsPipelineInfo.layout = pipelineLayout;
	graphicsPipelineInfo.renderPass = renderPass;
	graphicsPipelineInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Create Graphics Pipeline\n");
	}

}

void VulkanClass::createFramebuffers() {

	swapChain.framebuffers.resize(swapChain.imageViews.size());

	for (size_t i = 0; i < swapChain.framebuffers.size(); i++) {
		VkImageView attachments[] = { swapChain.imageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChain.extent.width;
		framebufferInfo.height = swapChain.extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapChain.framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed To Create Framebuffer\n");
		}

	}

}

void VulkanClass::recreateSwapChain() {

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logicalDevice);

	for (size_t i = 0; i < swapChain.framebuffers.size(); i++) {
		vkDestroyFramebuffer(logicalDevice, swapChain.framebuffers[i], nullptr);
	}
	for (size_t i = 0; i < swapChain.imageViews.size(); i++) {
		vkDestroyImageView(logicalDevice, swapChain.imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(logicalDevice, swapChain.__swapChain, nullptr);

	if (!checkSwapChainSupport(physicalDevice)) {
		throw std::runtime_error("SwapChain not Supported\n");
	}

	createSwapChain();
	createImageViews();
	createFramebuffers();

}

void VulkanClass::createCommandPool() {

	VkCommandPoolCreateInfo commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = QueueFamilyIndex.graphicsFamily;

	if (vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Create Command Pool\n");
	}

}

void VulkanClass::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index, uint32_t currentFrame) {

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Being Recording Command Buffer\n");
	}

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = swapChain.framebuffers[index];
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = swapChain.extent;
	VkClearValue clearColor = { {{0.2f, 0.3f, 0.3f, 1.0f}} };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;
	
	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChain.extent.width);
	viewport.height = static_cast<float>(swapChain.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	//vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissorRect{};
	scissorRect.extent = swapChain.extent;
	scissorRect.offset = { 0,0 };
	//vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);

	//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	//vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &transformDescriptorSet[currentFrame], 0, 0);

	vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Record Command Buffer\n");
	}

}

void VulkanClass::createCommandBuffer() {

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	commandBuffer.resize(swapChain.MAX_FRAMES_IN_FLIGHT);

	for (size_t i=0; i<swapChain.MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed To Allocate Command Buffer\n");
		}
	}

}

void VulkanClass::createSyncObjects() {

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	imageAvailableSemaphore.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
	renderFinishedSempahore.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
	inFlightFence.resize(swapChain.MAX_FRAMES_IN_FLIGHT);

	for (size_t i=0; i<swapChain.MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSempahore[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFence[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed To Create Sync Objects\n");
		}
	}

	vkCreateFence(logicalDevice, &fenceInfo, nullptr, &imGuiFence);

}

void VulkanClass::draw(uint32_t& imageIndex) {

	uint32_t index;

	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain.__swapChain, UINT32_MAX, imageAvailableSemaphore[imageIndex], VK_NULL_HANDLE, &index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		std::cout << "NO WORK SUBMITTED\n";
		return;
	}

	vkResetFences(logicalDevice, 1, &inFlightFence[imageIndex]);

	vkResetCommandBuffer(commandBuffer[imageIndex], 0);

	recordCommandBuffer(commandBuffer[imageIndex], index, imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore[imageIndex]};
	VkSemaphore signalSemaphores[] = { renderFinishedSempahore[imageIndex]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer[imageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("Failed To Submit Draw Command\n");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	
	VkSwapchainKHR swapChains[] = { swapChain.__swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &index;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
		//std::cout<<"recreated swapchain\n";
	}

}

//void VulkanClass::initImGui() {
//
//	VkDescriptorPoolSize pool_sizes[] =
//	{
//		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
//		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
//		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
//		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
//		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
//		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
//		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
//		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
//		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
//		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
//		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
//	};
//
//	VkDescriptorPoolCreateInfo pool_info = {};
//	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//	pool_info.maxSets = 1000;
//	pool_info.poolSizeCount = std::size(pool_sizes);
//	pool_info.pPoolSizes = pool_sizes;
//
//	if (vkCreateDescriptorPool(logicalDevice, &pool_info, nullptr, &imguiDescriptorPool) != VK_SUCCESS) {
//		throw std::runtime_error("Failed to Create ImGui Descriptor Pool\n");
//	}
//
//	ImGui::CreateContext();
//
//	ImGui_ImplGlfw_InitForVulkan(window, false);
//
//	ImGui_ImplVulkan_InitInfo init_info = {};
//	init_info.Instance = instance;
//	init_info.PhysicalDevice = physicalDevice;
//	init_info.Device = logicalDevice;
//	init_info.Queue = graphicsQueue;
//	init_info.DescriptorPool = imguiDescriptorPool;
//	init_info.MinImageCount = 3;
//	init_info.ImageCount = 3;
//	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
//
//	VkCommandBuffer commandBuffer;
//	VkCommandBufferAllocateInfo imguiCmdBufferInfo{};
//	imguiCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	imguiCmdBufferInfo.commandPool = commandPool;
//	imguiCmdBufferInfo.commandBufferCount = 1;
//	
//	vkAllocateCommandBuffers(logicalDevice, &imguiCmdBufferInfo, &commandBuffer);
//
//	ImGui_ImplVulkan_Init(&init_info, renderPass);
//
//	vkResetFences(logicalDevice, 1, &imGuiFence);
//	vkResetCommandBuffer(commandBuffer, 0);
//
//	VkCommandBufferBeginInfo cmdBeginInfo{};
//	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//	vkBeginCommandBuffer(commandBuffer, &cmdBeginInfo);
//
//	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
//
//	vkEndCommandBuffer(commandBuffer);
//
//	VkSubmitInfo cmdinfo{};
//	cmdinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	cmdinfo.commandBufferCount = 1;
//	cmdinfo.pCommandBuffers = &commandBuffer;
//
//	vkQueueSubmit(graphicsQueue, 1, &cmdinfo, imGuiFence);
//
//	vkWaitForFences(logicalDevice, 1, &imGuiFence, true, UINT64_MAX);
//
//	// clear font textures from cpu data
//	ImGui_ImplVulkan_DestroyFontUploadObjects();
//
//}
//
//void VulkanClass::drawGui() {
//
//	ImGui_ImplVulkan_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//
//	ImGui::NewFrame();
//
//	ImGui::ShowDemoWindow();
//
//	ImGui::Render();
//
//}

void VulkanClass::loadModel() {

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
		std::cout << warn + err << "\n";
		throw std::runtime_error(warn + err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				-attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertices.push_back(vertex);
			indices.push_back(indices.size());
		}
	}

}

void VulkanClass::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);

}

void VulkanClass::createVertexBuffer() {

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.size = sizeof(Vertex) * vertices.size();
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Vertex Buffer\n");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, vertexBuffer, &memRequirements);

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &vertexBufferMemory);

	vkBindBufferMemory(logicalDevice, vertexBuffer, vertexBufferMemory, 0);

	vkMapMemory(logicalDevice, vertexBufferMemory, 0, bufferInfo.size, 0, &vertexBufferMap);
	memcpy(vertexBufferMap, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(logicalDevice, vertexBufferMemory);

}

void VulkanClass::createIndexBuffer() {

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.size = bufferSize;

	vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &stagingBuffer);

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(logicalDevice, stagingBuffer, &memReq);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &stagingBufferMemory);

	vkBindBufferMemory(logicalDevice, stagingBuffer, stagingBufferMemory, 0);

	void* data;
	vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	bufferInfo.size = bufferSize;

	vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &indexBuffer);

	vkGetBufferMemoryRequirements(logicalDevice, indexBuffer, &memReq);

	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &indexBufferMemory);

	vkBindBufferMemory(logicalDevice, indexBuffer, indexBufferMemory, 0);


	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);

}
