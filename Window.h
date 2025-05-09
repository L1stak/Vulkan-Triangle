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
		GLFWwindow* GetCurrentWindow();
	private:

		VkInstance  instance = {};
		GLFWwindow* window = nullptr;
		VkPhysicalDeviceProperties  GPU_info;
		VkSurfaceKHR surface;
	};

