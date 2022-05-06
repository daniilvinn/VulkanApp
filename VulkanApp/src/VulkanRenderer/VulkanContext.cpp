#include "VulkanContext.h"
#include <Core/Base.h>

#include <Core/Application.h>

#include <VulkanRenderer/VulkanLogger.h>
#include <VulkanRenderer/VulkanDevice.h>

#include <glfw/glfw3.h>

namespace vkapp {

	VulkanContext* VulkanContext::s_Instance = nullptr;
	QueueFamiliesInfo VulkanContext::s_Familiesinfo = QueueFamiliesInfo{};

	VulkanContext::VulkanContext(GLFWwindow* handle)
	{
		VKAPP_ASSERT(handle, "Handle pointer is null!");
		m_Handle = handle;
	}

	VulkanContext::~VulkanContext()
	{
		if (VKAPP_ENABLE_VULKAN_MESSENGER) {
			VulkanLogger::Shutdown(&m_VkInstance);
		}
		m_LogicalDevice->Destroy();
		m_Swapchain->DestroySurface();
		vkDestroyInstance(m_VkInstance, nullptr);
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

		// ========= Creating Physical and Logical Devices Stage ==========
		m_PhysicalDevice = VulkanPhysicalDevice::SelectDevice();

		// FIX: double VulkanPhysicalDevice constructor invoñation
		VkPhysicalDeviceFeatures enabledFeatures;
		memset(&enabledFeatures, 0, sizeof(enabledFeatures));
		enabledFeatures.geometryShader = true;
		m_LogicalDevice = std::make_shared<VulkanDevice>(m_PhysicalDevice, enabledFeatures);
		// ================================================================

		// ======== Creating swapchain and window surface stage ===========
		// HACK: using constants such as 1600 and 900 to create framebuffer specification, 
		// instead of retreiving window's size from variable
		SwapchainFramebufferSpecification framebufferSpecification = { 1600, 900, true };
		m_Swapchain = std::make_shared<VulkanSwapchain>(m_Handle, m_LogicalDevice, framebufferSpecification);

		// ================================================================
		
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

