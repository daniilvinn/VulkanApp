#include "VulkanAllocator.h"
#include <VulkanRenderer/VulkanContext.h>

#include <Core/Base.h>

namespace vkapp
{
	VulkanAllocator* VulkanAllocator::s_Instance = nullptr;
	ApplicationMemoryUsageStats VulkanAllocator::m_Stats = ApplicationMemoryUsageStats();

	VulkanAllocator::VulkanAllocator()
	{
		auto context = VulkanContext::Get();
		VmaVulkanFunctions vkfps = {};
		vkfps.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vkfps.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocatorinfo = {};
		allocatorinfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorinfo.instance = context->GetVkInstance();
		allocatorinfo.physicalDevice = *context->GetDevice()->GetPhysicalDevice();
		allocatorinfo.device = *context->GetDevice();
		allocatorinfo.pVulkanFunctions = &vkfps;

		VK_CHECK_RESULT(vmaCreateAllocator(&allocatorinfo, &m_Allocator));
	}

	VulkanAllocator::~VulkanAllocator()
	{
		vmaDestroyAllocator(m_Allocator);
	}

	void VulkanAllocator::Init()
	{
		VKAPP_ASSERT(s_Instance == nullptr, "VulkanAllocator is already initialized!");
		s_Instance = new VulkanAllocator();
	}

	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo bufferinfo ,VmaMemoryUsage usage, VkBuffer& buffer)
	{
		VmaAllocationCreateInfo allocCI = {};
		allocCI.usage = usage;

		VmaAllocation allocation;
		VmaAllocationInfo allocinfo;

		vmaCreateBuffer(m_Allocator, &bufferinfo, &allocCI, &buffer, &allocation, &allocinfo);

		VKAPP_LOG_TRACE("Creating Buffer: Total Allocated Memory ({0} + {1})", m_Stats.totalallocatedmemory, bufferinfo.size);
		m_Stats.totalallocatedmemory += bufferinfo.size;

		return allocation;

	}

	VmaAllocation VulkanAllocator::AllocateMemory(uint64_t bytes, VmaMemoryUsage usage)
	{
		VmaAllocation memory;
		VmaAllocationCreateInfo memoryinfo = {};
		memoryinfo.pool = VK_NULL_HANDLE;
		memoryinfo.usage = usage;

		VkMemoryRequirements requirements = {};
		requirements.size = bytes;

		vmaAllocateMemory(m_Allocator, &requirements, &memoryinfo, &memory, nullptr);

		return memory;
	}

	void VulkanAllocator::DestroyBuffer(VkBuffer& buffer, VmaAllocation& allocation)
	{
		auto device = *VulkanContext::Get()->GetDevice();

		VmaAllocationInfo allocinfo = {};
		vmaGetAllocationInfo(m_Allocator, allocation, &allocinfo);

		vkDeviceWaitIdle(device);
		vmaDestroyBuffer(m_Allocator, buffer, allocation);

		VKAPP_LOG_TRACE("Destroying Buffer: Total Allocated Memory ({0} - {1})", m_Stats.totalallocatedmemory, allocinfo.size);
		m_Stats.totalallocatedmemory -= allocinfo.size;
	}

	void VulkanAllocator::Free(VmaAllocation allocation)
	{
		VmaAllocationInfo allocinfo = {};
		vmaGetAllocationInfo(m_Allocator, allocation, &allocinfo);

		vmaFreeMemory(m_Allocator, allocation);

		VKAPP_LOG_TRACE("Freeing Memory: Total Allocated Memory ({0} - {1})", m_Stats.totalallocatedmemory, allocinfo.size);
		m_Stats.totalallocatedmemory -= allocinfo.size;
	}
}
