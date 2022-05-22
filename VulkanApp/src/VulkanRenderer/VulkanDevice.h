#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <unordered_set>

namespace vkapp {

	class VulkanPhysicalDevice {
	public:
		struct QueueFamilyIndices {
			int32_t graphicsQueue = -1;
			int32_t transferQueue = -1;
			int32_t computeQueue = -1;
		};

		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();
		inline static std::shared_ptr<VulkanPhysicalDevice> SelectDevice() { return std::make_shared<VulkanPhysicalDevice>(); }
		
	public:
		VkPhysicalDevice GetCurrentDevice() { return m_PhysicalDevice; }
		inline const QueueFamilyIndices& GetQueueFamilyIndices() { return m_QueueFamilyIndices; };
		inline const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProps() { return m_QueueFamilyProperties; };
		inline bool CheckExtensionPresent(const std::string& extension);

	public:
		operator VkPhysicalDevice() { return m_PhysicalDevice; };

	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		QueueFamilyIndices m_QueueFamilyIndices;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;

		std::vector<VkExtensionProperties> m_PhysicalDeviceExtensions;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::unordered_set<std::string> m_SupportedExtensions;

	private:
		const float defaultQueuePriority = 1.0f;
		friend class VulkanDevice;
	};



	class VulkanDevice
	{
	public:
		VulkanDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
		~VulkanDevice();

		void Destroy();

	public:
		inline std::shared_ptr<VulkanPhysicalDevice> GetPhysicalDevice() { return m_PhysicalDevice; }
		inline VkQueue* GetGraphicsQueue() { return &m_GraphicsQueue; }
		inline VkQueue* GetComputeQueue() { return &m_ComputeQueue; }
		inline VkQueue* GetTransferQueue() { return &m_TransferQueue; }

	public:
		operator VkDevice() { return m_Device; }

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;

		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;
		VkQueue m_TransferQueue;
	};

}