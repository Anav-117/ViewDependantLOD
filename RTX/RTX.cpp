#include "VKConfig.h"
#include <iostream>

VulkanClass* vk;

namespace win {
	int width = 1920;
	int height = 1080;
}

namespace camera {
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 fwd = glm::vec3(0.0f, 0.0f, 1.0f);
}

Transform transform;

namespace hostSwapChain {
	uint32_t currentFrame = 0;
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, true);
	}
	if (key == GLFW_KEY_A) {
		camera::pos += glm::vec3(-0.1f, 0.0f, 0.0f);
	}
	if (key == GLFW_KEY_D) {
		camera::pos += glm::vec3(0.1f, 0.0f, 0.0f);
	}
	if (key == GLFW_KEY_Q) {
		camera::pos += glm::vec3(0.0f, 0.1f, 0.0f);
	}
	if (key == GLFW_KEY_E) {
		camera::pos += glm::vec3(0.0f, -0.1f, 0.0f);
	}
	if (key == GLFW_KEY_W) {
		camera::pos += glm::vec3(0.0f, 0.0f, 0.1f);
	}
	if (key == GLFW_KEY_S) {
		camera::pos += glm::vec3(0.0f, 0.0f, -0.1f);
	}

}

void windowResizeCallback(GLFWwindow* window, int width, int height) {

	vk->framebufferResized = true;

}

void display() {

	vkWaitForFences(vk->getLogicalDevice(), 1, &vk->inFlightFence[hostSwapChain::currentFrame], VK_TRUE, UINT32_MAX);

	vk->draw(hostSwapChain::currentFrame);

	hostSwapChain::currentFrame = (hostSwapChain::currentFrame + 1) % vk->getMaxFramesInFlight();

	//vk->drawGui();

}

void idle() {

	transform.M = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) * glm::mat4(1.0f);
	transform.V = glm::lookAt(camera::pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	transform.P = glm::perspective(glm::radians(45.0f), win::width / (float)win::height, 0.1f, 100.0f);

	transform.cameraPos = camera::pos;

	vk->transform = transform;

	vk->updateTransform();

}

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(win::width, win::height, "RTX", 0, nullptr);

	vk = new VulkanClass(window);
	vk->createTransformBuffer(sizeof(transform));
	vk->createTransformDescriptorSet();

	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetWindowSizeCallback(window, windowResizeCallback);

	while (!glfwWindowShouldClose(window)) {

		idle();
		display();

		glfwPollEvents();

	}

	vkDeviceWaitIdle(vk->getLogicalDevice());

	delete vk;

	return 0;

}