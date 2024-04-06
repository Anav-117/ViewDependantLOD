#include "VKConfig.h"
#include <iostream>

namespace win {
	int width = 3840;
	int height = 2160;
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, true);
	}

}

void display(VulkanClass& vk) {

	vkWaitForFences(vk.getLogicalDevice(), 1, &vk.inFlightFence, VK_TRUE, UINT32_MAX);
	vkResetFences(vk.getLogicalDevice(), 1, &vk.inFlightFence);

	uint32_t imageIndex;
	vk.draw(imageIndex);

}

void idle() {

}

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(win::width, win::height, "RTX", 0, nullptr);

	VulkanClass vk(window);

	glfwSetKeyCallback(window, keyboardCallback);

	while (!glfwWindowShouldClose(window)) {

		idle();
		display(vk);

		glfwPollEvents();

	}

	vkDeviceWaitIdle(vk.getLogicalDevice());

	return 0;

}