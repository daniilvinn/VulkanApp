#pragma once

#include <vulkan/vulkan.h>
#include <VulkanRenderer/VulkanPipeline.h>
#include <VulkanRenderer/VulkanVertexBuffer.h>

namespace vkapp {

	// ULTRA basic Vulkan Renderer.

	class VulkanRenderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginRenderPass(VkRenderPass renderpass, VkFramebuffer framebuffer);
		static void EndRenderPass();

		static void Submit(VulkanPipeline& pipeline);
		static void Submit(VulkanPipeline pipeline, VulkanVertexBuffer vbo);

	private:
		static VkDevice m_Device;
		static VkCommandPool m_CommandPool;

	};
}