#pragma once

#include <Core/Base.h>
#include <vulkan/vulkan.h>

namespace vkapp {
	class VulkanPipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline();

		VkPipeline GetPipeline() { return m_Pipeline; };

	private:
		void SetRenderPass();

	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_Layout;
		VkRenderPass m_RenderPass;

	};
}