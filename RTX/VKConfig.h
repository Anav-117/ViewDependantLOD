#pragma once

#ifndef VK_CONFIG
	#define VK_CONFIG
#endif

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

#include "Shaders.h"

struct QueueFamily {

	uint32_t graphicsFamily;
	uint32_t presentFamily;

};

struct SwapChainSupport {

	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

};

struct SwapChain {

	VkSwapchainKHR __swapChain;

	VkSurfaceFormatKHR format;
	VkPresentModeKHR presentMode;
	VkExtent2D extent;

	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
 
	VkSurfaceFormatKHR findSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR findSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

};

class VulkanClass {

private:

	bool enableValidationLayers = true;
	std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	QueueFamily QueueFamilyIndex;
	SwapChainSupport SwapChainDetails;

	VkInstance instance;

	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;
	GLFWwindow* window;

	SwapChain swapChain;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

public:

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSempahore;
	VkFence inFlightFence;

	VulkanClass();
	VulkanClass(GLFWwindow* win);
	~VulkanClass();

	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
	bool findQueueFamilies(VkPhysicalDevice device);
	bool checkSwapChainSupport(VkPhysicalDevice device);
	VkDevice getLogicalDevice() { return logicalDevice; }
	void draw(uint32_t& imageIndex);

	//void initVulkan();
	void createInstance();
	VkPhysicalDevice findPhysicalDevice();
	void createLogicalDevice();
	void createSurface();

	void createSwapChain();
	void createImageViews();

	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();

	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void createSyncObjects();

};