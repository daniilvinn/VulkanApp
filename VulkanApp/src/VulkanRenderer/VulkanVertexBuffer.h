#pragma once

#include <VulkanRenderer/VulkanAllocator.h>

namespace vkapp {

	class VulkanVertexBuffer
	{
	public:
		VulkanVertexBuffer(uint64_t size);
		~VulkanVertexBuffer();


	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_AllocHandle = VK_NULL_HANDLE;
	};

}