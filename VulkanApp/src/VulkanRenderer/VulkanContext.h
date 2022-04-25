#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>

namespace vkapp {

	class VulkanContext {
	public:
		VulkanContext(GLFWwindow* handle);
		~VulkanContext();

		void Init();

		VkInstance* GetVkInstance() { return &m_VkInstance; }
		// VkPhysicalDevice GetPhysicalDevice(); TO BE IMPLEMENTED

	private:
		std::vector<const char*> GetRequiredExtensions();
		std::vector<const char*> GetRequiredLayers();

	private:
		static VulkanContext* s_Instance;
		GLFWwindow* m_Handle;
		VkInstance m_VkInstance;
	};

}