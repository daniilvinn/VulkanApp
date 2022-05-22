#include "VulkanRenderer.h"

#include <VulkanRenderer/VulkanContext.h>
#include <VulkanRenderer/VulkanDevice.h>

namespace vkapp {

	static struct RendererData
	{
		std::shared_ptr<VulkanDevice> m_Device;
		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;
		std::shared_ptr<VulkanSwapchain> m_Swapchain;
	} s_Data;

	void VulkanRenderer::Init()
	{
		auto device = VulkanContext::Get()->GetDevice();
		s_Data.m_Device = VulkanContext::Get()->GetDevice();

		VkCommandPoolCreateInfo commandpoolCI = {};
		commandpoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandpoolCI.queueFamilyIndex = device->GetPhysicalDevice()->GetQueueFamilyIndices().graphicsQueue;
		commandpoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_CHECK_RESULT(vkCreateCommandPool(*s_Data.m_Device, &commandpoolCI, nullptr, &s_Data.m_CommandPool));

		VkCommandBufferAllocateInfo cmdbufferallocinfo = {};
		cmdbufferallocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdbufferallocinfo.commandPool = s_Data.m_CommandPool;
		cmdbufferallocinfo.commandBufferCount = 1;
		cmdbufferallocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		vkAllocateCommandBuffers(*s_Data.m_Device, &cmdbufferallocinfo, &s_Data.m_CommandBuffer);

		s_Data.m_Swapchain = VulkanContext::Get()->GetSwapchain();

		VKAPP_LOG_TRACE("[Renderer]: Initializing complete!");

	}

	void VulkanRenderer::Shutdown()
	{
		vkFreeCommandBuffers(*s_Data.m_Device, s_Data.m_CommandPool, 1, &s_Data.m_CommandBuffer);
		vkDestroyCommandPool(*s_Data.m_Device, s_Data.m_CommandPool, nullptr);
	}

	void VulkanRenderer::BeginRenderPass(VkRenderPass renderpass, VkFramebuffer framebuffer)
	{
		auto swapchain = VulkanContext::Get()->GetSwapchain();

		VkCommandBufferBeginInfo cmdbufferbegininfo = {};
		cmdbufferbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(s_Data.m_CommandBuffer, &cmdbufferbegininfo);

		VkClearValue vkclearvalue = {};
		vkclearvalue.color = {1.0f, 0.0f, 0.0f, 1.0f};

		VkRenderPassBeginInfo begininfo = {};
		begininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		begininfo.renderPass = renderpass;
		begininfo.clearValueCount = 1;
		begininfo.pClearValues = &vkclearvalue;
		begininfo.renderArea.offset = { 0, 0 };
		begininfo.renderArea.extent = swapchain->GetExtent();
		begininfo.framebuffer = framebuffer;

		vkCmdBeginRenderPass(s_Data.m_CommandBuffer, &begininfo, VK_SUBPASS_CONTENTS_INLINE);
		VkViewport viewport = {0, 0, 1600, 900, 1.0, 1.0f};
		vkCmdSetViewport(s_Data.m_CommandBuffer, 0, 1, &viewport);

	}

	void VulkanRenderer::EndRenderPass()
	{
		vkCmdEndRenderPass(s_Data.m_CommandBuffer);
		vkEndCommandBuffer(s_Data.m_CommandBuffer);

		VkPipelineStageFlags stagemasks[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitinfo = {};
		submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitinfo.commandBufferCount = 1;
		submitinfo.pCommandBuffers = &s_Data.m_CommandBuffer;
		submitinfo.signalSemaphoreCount = 1;
		submitinfo.pSignalSemaphores = &s_Data.m_Swapchain->m_Semaphores.renderComplete;
		submitinfo.waitSemaphoreCount = 1;
		submitinfo.pWaitSemaphores = &s_Data.m_Swapchain->m_Semaphores.presentComplete;
		submitinfo.pWaitDstStageMask = stagemasks;

		vkQueueSubmit(*s_Data.m_Device->GetGraphicsQueue(), 1, &submitinfo, s_Data.m_Swapchain->m_Fence);
		
	}

	void VulkanRenderer::Submit(VulkanPipeline& pipeline)
	{
		vkCmdBindPipeline(s_Data.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());
		vkCmdDraw(s_Data.m_CommandBuffer, 3, 1, 0, 0);
	};
}