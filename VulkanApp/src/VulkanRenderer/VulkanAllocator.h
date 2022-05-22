#pragma once

#include <vk_mem_alloc.h>
#include <stdexcept>

namespace vkapp {

	struct ApplicationMemoryUsageStats
	{
		uint64_t totalallocatedmemory = 0;
		// ...
	};

	class VulkanAllocator
	{
	public:
		VulkanAllocator();
		~VulkanAllocator();

		static void Init();

		static VulkanAllocator* Get() { return s_Instance; }

		VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferinfo, VmaMemoryUsage usage, VkBuffer& buffer);
		VmaAllocation AllocateImage(VkImage image, uint32_t size) { throw std::logic_error("Not Implemented Method Usage"); };
		VmaAllocation AllocateMemory(uint64_t bytes, VmaMemoryUsage usage);

		void DestroyBuffer(VkBuffer& buffer, VmaAllocation& allocation);
		void Free(VmaAllocation allocation);

	private:
		static VulkanAllocator* s_Instance;
		VmaAllocator m_Allocator;
		static ApplicationMemoryUsageStats m_Stats;
	};

}
