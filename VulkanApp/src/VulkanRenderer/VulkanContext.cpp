#include "VulkanContext.h"
#include <Core/Base.h>
#include <VulkanRenderer/VulkanLogger.h>

namespace vkapp {

	VulkanContext* VulkanContext::s_Instance = nullptr;

	VulkanContext::VulkanContext(GLFWwindow* handle)
	{
		VKAPP_ASSERT(handle, "Handle pointer is null!");
		m_Handle = handle;
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyInstance(m_VkInstance, nullptr);
		if (VKAPP_ENABLE_VULKAN_MESSENGER) {
			VulkanLogger::Shutdown(&m_VkInstance);
		}
	}

	void VulkanContext::Init()
	{
		s_Instance = this;

		// =================== Vulkan Instance Init Stage =================
		std::vector<const char*> addedExtensions = GetRequiredExtensions();
		std::vector<const char*> addedLayers = GetRequiredLayers();

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan Application";
		appInfo.apiVersion = VK_API_VERSION_1_3;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "VulkanEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pNext = nullptr;

		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(addedExtensions.size());
		instanceInfo.ppEnabledExtensionNames = addedExtensions.data();
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(addedLayers.size());
		instanceInfo.ppEnabledLayerNames = addedLayers.data();
		instanceInfo.pNext = nullptr;

		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_VkInstance);
		VKAPP_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan Instance!");
		// ================================================================

		// =================== Vulkan Messager Init Stage =================
		if (VKAPP_ENABLE_VULKAN_MESSENGER) VulkanLogger::Init(&m_VkInstance);
		// ================================================================


		// TODO
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physical_devices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, physical_devices.data());
		
	}

	std::vector<const char*> VulkanContext::GetRequiredExtensions()
	{
		uint32_t count;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);

		if (VKAPP_ENABLE_VULKAN_VALIDATION_LAYERS) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	std::vector<const char*> VulkanContext::GetRequiredLayers()
	{
		std::vector<const char*> layers;
		if (VKAPP_ENABLE_VULKAN_VALIDATION_LAYERS) {
			layers.push_back("VK_LAYER_KHRONOS_validation");
		}
		return layers;
	}
}