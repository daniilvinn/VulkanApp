#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <VulkanRenderer/VulkanDevice.h>

namespace vkapp {

	class VulkanSwapchain {
	public:
		VulkanSwapchain(GLFWwindow* handle, std::shared_ptr<VulkanDevice> device);
		~VulkanSwapchain();

		void DestroySurface();
		void DestroySwapchain();

		VkExtent2D GetExtent() { return m_FramebuffersSize; }
		VkFormat GetSwapchainImageFormat() { return m_SurfaceFormat.format; }
		VkRenderPass GetRenderPass() { return m_RenderPass; }
		VkFramebuffer GetCurrentFramebuffer() { return m_Framebuffers[m_CurrentImageIndex]; }

		void BeginFrame();
		void EndFrame();
		void Present();

	private:
		VkSurfaceFormatKHR SelectSurfaceFormat();
		VkPresentModeKHR SelectPresentMode();
		VkExtent2D SelectExtent();

	private:
		GLFWwindow* m_WindowHandle;
		VkInstance m_VkInstance;
		std::shared_ptr<VulkanDevice> m_Device;

	private:
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;
		VkExtent2D m_FramebuffersSize;
		uint32_t m_CurrentImageIndex;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VkFramebuffer> m_Framebuffers;

		VkSurfaceKHR m_Surface;

		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;

		struct SwapchainDetails {
			VkSurfaceCapabilitiesKHR surface_capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> present_modes;
		} m_SwapchainDetails;

		struct Semaphores
		{
			VkSemaphore renderComplete;
			VkSemaphore presentComplete;
		} m_Semaphores;
		VkFence m_Fence;

		VkRenderPass m_RenderPass;

		friend class VulkanRenderer;
	};

}