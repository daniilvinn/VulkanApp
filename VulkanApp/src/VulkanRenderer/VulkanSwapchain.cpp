#include "VulkanSwapchain.h"

#include <Core/Base.h>
#include <VulkanRenderer/VulkanContext.h>

namespace vkapp {

	VulkanSwapchain::VulkanSwapchain(GLFWwindow* handle, std::shared_ptr<VulkanDevice> device) : m_WindowHandle(handle), m_Device(device)
	{
		// Creating surface
		
		m_VkInstance = VulkanContext::Get()->GetVkInstance();
		VK_CHECK_RESULT(glfwCreateWindowSurface(m_VkInstance, m_WindowHandle, nullptr, &m_Surface));

		VkBool32 supportResult;
		vkGetPhysicalDeviceSurfaceSupportKHR(
			*m_Device->GetPhysicalDevice(),
			m_Device->GetPhysicalDevice()->GetQueueFamilyIndices().graphicsQueue,
			m_Surface,
			&supportResult
		);

		if (supportResult != true) 
			throw std::runtime_error("Chosen device doesn't support surface presentation!");

		
		// Surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*m_Device->GetPhysicalDevice(), m_Surface, &m_SwapchainDetails.surface_capabilities);

		// Swap chain image formats
		uint32_t formatsCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(*m_Device->GetPhysicalDevice(), m_Surface, &formatsCount, nullptr);
		m_SwapchainDetails.formats.resize(formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(*m_Device->GetPhysicalDevice(), m_Surface, &formatsCount, m_SwapchainDetails.formats.data());

		// Surface presentation modes
		uint32_t presentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(*m_Device->GetPhysicalDevice(), m_Surface, &presentModesCount, nullptr);
		m_SwapchainDetails.present_modes.resize(presentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(*m_Device->GetPhysicalDevice(), m_Surface, &presentModesCount, m_SwapchainDetails.present_modes.data());
		
		// Select surface format and present mode;
		m_SurfaceFormat = SelectSurfaceFormat();
		m_PresentMode = SelectPresentMode();
		m_FramebuffersSize = SelectExtent();


		// Swapchain Creating
		uint32_t swapchainImageCount = 0;
		swapchainImageCount = m_SwapchainDetails.surface_capabilities.minImageCount + 1;
		if (m_SwapchainDetails.surface_capabilities.maxImageCount > 0 && swapchainImageCount > m_SwapchainDetails.surface_capabilities.maxImageCount) {
			swapchainImageCount = m_SwapchainDetails.surface_capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR swapchainInfo = {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.pNext = nullptr;
		swapchainInfo.surface = m_Surface;
		swapchainInfo.minImageCount = swapchainImageCount;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageFormat = m_SurfaceFormat.format;
		swapchainInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainInfo.imageExtent = m_FramebuffersSize;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainInfo.queueFamilyIndexCount = 0;
		swapchainInfo.pQueueFamilyIndices = nullptr;
		swapchainInfo.presentMode = m_PresentMode;
		swapchainInfo.clipped = VK_TRUE;
		swapchainInfo.preTransform = m_SwapchainDetails.surface_capabilities.currentTransform;
		swapchainInfo.oldSwapchain = m_Swapchain;

		VK_CHECK_RESULT(vkCreateSwapchainKHR(*m_Device, &swapchainInfo, nullptr, &m_Swapchain));

		{ 
			uint32_t count;
			vkGetSwapchainImagesKHR(*m_Device, m_Swapchain, &count, nullptr);
			m_SwapchainImages.resize(count);
			vkGetSwapchainImagesKHR(*m_Device, m_Swapchain, &count, m_SwapchainImages.data());
		}

		m_SwapchainDetails.present_modes.clear();
		m_SwapchainDetails.formats.clear();

		VkAttachmentDescription attachment_description = {};
		attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment_description.format = m_SurfaceFormat.format;

		VkAttachmentReference attachment_reference = {};
		attachment_reference.attachment = 0;
		attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass_description = {};
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &attachment_reference;

		VkRenderPassCreateInfo renderpassinfo = {};
		renderpassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpassinfo.attachmentCount = 1;
		renderpassinfo.pAttachments = &attachment_description;
		renderpassinfo.subpassCount = 1;
		renderpassinfo.pSubpasses = &subpass_description;

		VK_CHECK_RESULT(vkCreateRenderPass(*m_Device, &renderpassinfo, nullptr, &m_RenderPass));

		m_ImageViews.resize(m_SwapchainImages.size());
		m_Framebuffers.resize(m_ImageViews.size());
		for (int i = 0; i < m_ImageViews.size(); i++) {
			VkImageViewCreateInfo createinfo;
			createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createinfo.pNext = nullptr;
			createinfo.flags = 0;
			createinfo.image = m_SwapchainImages[i];
			createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createinfo.format = m_SurfaceFormat.format;
			createinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createinfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createinfo.subresourceRange.baseMipLevel = 0;
			createinfo.subresourceRange.levelCount = 1;
			createinfo.subresourceRange.baseArrayLayer = 0;
			createinfo.subresourceRange.layerCount = 1;

			VK_CHECK_RESULT(vkCreateImageView(*m_Device, &createinfo, nullptr, &m_ImageViews[i]));

			VkFramebufferCreateInfo framebufferinfo = {};
			framebufferinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferinfo.renderPass = m_RenderPass;
			framebufferinfo.width = m_FramebuffersSize.width;
			framebufferinfo.height = m_FramebuffersSize.height;
			framebufferinfo.layers = 1;
			framebufferinfo.attachmentCount = 1;
			framebufferinfo.pAttachments = &m_ImageViews[i];

			VK_CHECK_RESULT(vkCreateFramebuffer(*m_Device, &framebufferinfo, nullptr, &m_Framebuffers[i]));

		}
		VKAPP_LOG_INFO("Swapchain Created! :)");

		// Allocating command buffers;
		VkCommandPoolCreateInfo commandpoolinfo = {};
		commandpoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandpoolinfo.queueFamilyIndex = device->GetPhysicalDevice()->GetQueueFamilyIndices().graphicsQueue;
		commandpoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		vkCreateCommandPool(*m_Device, &commandpoolinfo, nullptr, &m_CommandPool);
		

		VkCommandBufferAllocateInfo cmdbuffinfo = {};
		cmdbuffinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdbuffinfo.commandPool = m_CommandPool;
		cmdbuffinfo.commandBufferCount = 1;
		cmdbuffinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vkAllocateCommandBuffers(*m_Device, &cmdbuffinfo, &m_CommandBuffer);

		// Creating semaphores and fences
		VkSemaphoreCreateInfo semaphoreinfo = {};
		semaphoreinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreateSemaphore(*m_Device, &semaphoreinfo, nullptr, &m_Semaphores.presentComplete));
		VK_CHECK_RESULT(vkCreateSemaphore(*m_Device, &semaphoreinfo, nullptr, &m_Semaphores.renderComplete));

		VkFenceCreateInfo fenceinfo = {};
		fenceinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VK_CHECK_RESULT(vkCreateFence(*m_Device, &fenceinfo, nullptr, &m_Fence));
		
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		vkDestroySemaphore(*m_Device, m_Semaphores.presentComplete, nullptr);
		vkDestroySemaphore(*m_Device, m_Semaphores.renderComplete, nullptr);
		vkDestroyFence(*m_Device, m_Fence, nullptr);
	}

	void VulkanSwapchain::DestroySurface()
	{
		vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);
	}

	void VulkanSwapchain::DestroySwapchain()
	{
		vkDestroySwapchainKHR(*m_Device, m_Swapchain, nullptr);
	}

	void VulkanSwapchain::BeginFrame()
	{
		vkWaitForFences(*m_Device, 1, &m_Fence, VK_TRUE, UINT64_MAX);
		vkResetFences(*m_Device, 1, &m_Fence);
		vkResetCommandPool(*m_Device, m_CommandPool, 0);
		vkAcquireNextImageKHR(*m_Device, m_Swapchain, UINT64_MAX, m_Semaphores.presentComplete, VK_NULL_HANDLE, &m_CurrentImageIndex);
	}

	void VulkanSwapchain::Present()
	{
		VkPresentInfoKHR presentinfo = {};
		presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentinfo.pImageIndices = &m_CurrentImageIndex;
		presentinfo.swapchainCount = 1;
		presentinfo.pSwapchains = &m_Swapchain;
		presentinfo.waitSemaphoreCount = 1;
		presentinfo.pWaitSemaphores = &m_Semaphores.renderComplete;
		presentinfo.pResults = nullptr;

		vkQueuePresentKHR(*m_Device->GetGraphicsQueue(), &presentinfo);
	}

	VkSurfaceFormatKHR VulkanSwapchain::SelectSurfaceFormat()
	{
		for (const auto& format : m_SwapchainDetails.formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return format;
			}
		}
		VKAPP_LOG_WARN("Available 32-bit RGBA format with non-linear SRGB color space wasn't found");
		return m_SwapchainDetails.formats[0];
	}

	VkPresentModeKHR VulkanSwapchain::SelectPresentMode()
	{
		for (const auto& mode : m_SwapchainDetails.present_modes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) return mode;
		}
		VKAPP_LOG_WARN("Device doesn't support mailbox-mode presentation, selecting FIFO mode");
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::SelectExtent()
	{
		if (m_SwapchainDetails.surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::min()) {
			return m_SwapchainDetails.surface_capabilities.currentExtent;
		}
		else {
			int width;
			int height;
			glfwGetFramebufferSize(m_WindowHandle, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, m_SwapchainDetails.surface_capabilities.minImageExtent.width, m_SwapchainDetails.surface_capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, m_SwapchainDetails.surface_capabilities.minImageExtent.height, m_SwapchainDetails.surface_capabilities.maxImageExtent.height);

			return actualExtent;

		}
	}

}