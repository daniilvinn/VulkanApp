#include "VulkanVertexBuffer.h"
#include <VulkanRenderer/VulkanAllocator.h>
#include <VulkanRenderer/VulkanContext.h>

namespace vkapp
{
	VulkanVertexBuffer::VulkanVertexBuffer(uint64_t size)
	{
		auto allocator = VulkanAllocator::Get();

		VkBufferCreateInfo bufferCI = {};
		bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCI.size = size;

		m_AllocHandle = allocator->AllocateBuffer(bufferCI, VMA_MEMORY_USAGE_CPU_TO_GPU, m_Buffer);

	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		auto allocator = VulkanAllocator::Get();
		allocator->DestroyBuffer(m_Buffer, m_AllocHandle);
	}
}
