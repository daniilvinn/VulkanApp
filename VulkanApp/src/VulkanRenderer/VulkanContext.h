#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <VulkanRenderer/VulkanDevice.h>
#include <VulkanRenderer/VulkanSwapchain.h>

#include <utility>
#include <vector>
#include <memory>


namespace vkapp {

	using QueueFamilyInfo = std::pair<uint32_t, uint32_t>;

	// Queue families info. First component of a pair represents queue family index
	// and second component represents amount of queues in given family.
	struct QueueFamiliesInfo {
		QueueFamilyInfo graphicsQueues;
		QueueFamilyInfo computeQueues;
		QueueFamilyInfo transferQueues;
	};

	class VulkanContext {
	public:
		VulkanContext(GLFWwindow* handle);
		~VulkanContext();

		void Init();
		static VulkanContext* Get() { return s_Instance; };

		static VkInstance GetVkInstance() { return s_Instance->m_VkInstance; }
		std::shared_ptr<VulkanDevice> GetDevice() { return std::make_shared<VulkanDevice>(*m_LogicalDevice); };
		std::shared_ptr<VulkanSwapchain> GetSwapchain() { return m_Swapchain; };


	private:
		std::vector<const char*> GetRequiredExtensions();
		std::vector<const char*> GetRequiredLayers();

	private:
		static VulkanContext* s_Instance;
		static QueueFamiliesInfo s_Familiesinfo;
		GLFWwindow* m_Handle;
		VkInstance m_VkInstance;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<VulkanDevice> m_LogicalDevice;

		std::shared_ptr<VulkanSwapchain> m_Swapchain;

	};

}