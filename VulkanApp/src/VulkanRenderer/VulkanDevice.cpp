#include "VulkanDevice.h"

#include <vector>
#include <Core/Base.h>
#include "VulkanContext.h"
#include <Core/Logger.h>


namespace vkapp {

	// --- Vulkan Physical Device ---------------
	// ------------------------------------------

	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		// Selecting Physical device
		// Device is REQUIRED to be discrete GPU, support geometry and tesselation shader

		auto vkinstance = VulkanContext::GetVkInstance();
		{
			m_PhysicalDevice = VK_NULL_HANDLE;
			uint32_t amountOfPhysicalDevices = 0;
			vkEnumeratePhysicalDevices(vkinstance, &amountOfPhysicalDevices, nullptr);
			std::vector<VkPhysicalDevice> availableDevices(amountOfPhysicalDevices);
			vkEnumeratePhysicalDevices(vkinstance, &amountOfPhysicalDevices, availableDevices.data());
			VKAPP_LOG_INFO("Amount of available GPUs: {0}", availableDevices.size());

			VKAPP_LOG_INFO("~~~Available GPUs info~~~", availableDevices.size());
			for (auto& device : availableDevices) {
				short count = 1;
				VkPhysicalDeviceFeatures device_features;
				VkPhysicalDeviceProperties device_props;
				vkGetPhysicalDeviceFeatures(device, &device_features);
				vkGetPhysicalDeviceProperties(device, &device_props);

				// Checking device suitability
				if (device_props.deviceType ==
					VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
					device_features.geometryShader &&
					device_features.tessellationShader
					) {
					VKAPP_LOG_INFO("{0}.", count);
					VKAPP_LOG_TRACE("Device name: {0}", device_props.deviceName);
					VKAPP_LOG_TRACE("Device ID: {0}", device_props.deviceID);
					VKAPP_LOG_TRACE("Maximum allocation count: {0}", device_props.limits.maxMemoryAllocationCount);
					m_PhysicalDevice = device;
				}

				count++;
			}
		}

		// Retrieving physical device memory properties
		{
			vkGetPhysicalDeviceMemoryProperties(*this, &m_MemoryProperties);
		}

		// Retrieving supported physical device extensions
		{
			uint32_t count = 0;
			vkEnumerateDeviceExtensionProperties(*this, nullptr, &count, nullptr);

			if (count > 0) {
				m_PhysicalDeviceExtensions.resize(count);
				vkEnumerateDeviceExtensionProperties(*this, nullptr, &count, m_PhysicalDeviceExtensions.data());

				VKAPP_LOG_TRACE("Selected Physical Device has {0} extensions: ", m_PhysicalDeviceExtensions.size());

				for (const auto& ext : m_PhysicalDeviceExtensions) {
					VKAPP_LOG_TRACE("     {0}", ext.extensionName);
					m_SupportedExtensions.emplace(ext.extensionName);
				}
			}
		}
	
		// Filling in QueueFamilyIndices and QueueFamilyProperties structures
		{
			uint32_t count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
			m_QueueFamilyProperties.resize(count);
			vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, m_QueueFamilyProperties.data());

			uint32_t index = 0;

			for (const auto& family : m_QueueFamilyProperties) {
				if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					m_QueueFamilyIndices.graphicsQueue = index;
				}	
				else if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
					m_QueueFamilyIndices.computeQueue = index;
				}
				else if (family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
					m_QueueFamilyIndices.transferQueue = index;
				}
				index++;
			}

		}

		// Creating VkDeviceQueueCreateInfo's
		{
			// Graphics and presentation queue
			{

				VkDeviceQueueCreateInfo graphicsQueueInfo = {};
				graphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				graphicsQueueInfo.pQueuePriorities = &defaultQueuePriority;
				graphicsQueueInfo.queueCount = 1;
				graphicsQueueInfo.queueFamilyIndex = m_QueueFamilyIndices.graphicsQueue;
				graphicsQueueInfo.pNext = nullptr;
				m_QueueCreateInfos.push_back(graphicsQueueInfo);
			}

			// Compute Queue
			if (m_QueueFamilyIndices.computeQueue != m_QueueFamilyIndices.graphicsQueue) {
				VkDeviceQueueCreateInfo computeQueueInfo = {};
				computeQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; 
				computeQueueInfo.pQueuePriorities = &defaultQueuePriority;
				computeQueueInfo.queueCount = 1;
				computeQueueInfo.queueFamilyIndex = m_QueueFamilyIndices.computeQueue;
				computeQueueInfo.pNext = nullptr;
				m_QueueCreateInfos.push_back(computeQueueInfo);
			}

			// Transfer Queue
			if ((m_QueueFamilyIndices.transferQueue != m_QueueFamilyIndices.graphicsQueue) && (m_QueueFamilyIndices.transferQueue != m_QueueFamilyIndices.computeQueue)) {
				VkDeviceQueueCreateInfo transferQueueInfo = {};
				transferQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				transferQueueInfo.pQueuePriorities = &defaultQueuePriority;
				transferQueueInfo.queueCount = 1;
				transferQueueInfo.queueFamilyIndex = m_QueueFamilyIndices.transferQueue;
				transferQueueInfo.pNext = nullptr;
				m_QueueCreateInfos.push_back(transferQueueInfo);
			}
		}

	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
		
	}

	bool VulkanPhysicalDevice::CheckExtensionPresent(const std::string& extension)
	{
		return m_SupportedExtensions.find(extension) != m_SupportedExtensions.end();
	}

	// --------- Vulkan Device ------------------
	// ------------------------------------------
	VulkanDevice::VulkanDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
	{
		m_PhysicalDevice = physicalDevice;

		std::vector<const char*> deviceExtensions;
		if (m_PhysicalDevice->CheckExtensionPresent(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
			deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}
		//if (m_PhysicalDevice->CheckExtensionPresent(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME)) {
		//	deviceExtensions.push_back(VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME);
		//}

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
		deviceCreateInfo.queueCreateInfoCount = m_PhysicalDevice->m_QueueCreateInfos.size();
		deviceCreateInfo.pQueueCreateInfos = m_PhysicalDevice->m_QueueCreateInfos.data();
		if (deviceExtensions.size() > 0) {
			deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice->GetCurrentDevice(), &deviceCreateInfo, nullptr, &m_Device));

		vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetQueueFamilyIndices().graphicsQueue, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetQueueFamilyIndices().computeQueue, 0, &m_ComputeQueue);
		vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetQueueFamilyIndices().transferQueue, 0, &m_TransferQueue);

		VKAPP_LOG_INFO("Created logical device");
	}

	VulkanDevice::~VulkanDevice()
	{
		
	}

	void VulkanDevice::Destroy()
	{
		vkDeviceWaitIdle(m_Device);
		vkDestroyDevice(m_Device, nullptr);
		VKAPP_LOG_INFO("Logical device was destroyed!");
	}

}