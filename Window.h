#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>


//#include <vulkan/vulkan.h>
	class Window
	{

	public:
		Window(const char* title);
		~Window();
		void Display();
		void LoadShaders();
		VkQueue GetGraphicsQueue();
		VkQueue GetPresentQueue();
		GLFWwindow* GetCurrentWindow();
		VkDevice GetLogicalDevice();
		VkSwapchainKHR GetSwapChain();
		std::vector<VkFence> GetinFlightFences();
		std::vector<VkCommandBuffer> GetCommandBuffer();
		std::vector<VkSemaphore> GetImageAvalibleSemaphores();
	private:

		VkInstance  instance = {};
		GLFWwindow* window = nullptr;
		VkPhysicalDeviceProperties  GPU_info;
		VkSurfaceKHR surface;
		VkDevice logicalDevice;
		VkRenderPass renderPass;
		VkShaderModule vertModule;
		VkShaderModule fragModule;
		VkPipeline graphicsPipeline = NULL;
		VkPipelineLayout pipelineLayout;
		VkCommandPool commandPool;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSwapchainKHR swapChain;
	};

