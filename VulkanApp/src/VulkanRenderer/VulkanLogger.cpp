#include "VulkanLogger.h"
#include <Core/Logger.h>
#include <Core/Base.h>

namespace vkapp {

	VkDebugUtilsMessengerEXT* VulkanLogger::m_Logger = nullptr;

	void VulkanLogger::Init(VkInstance* instance)
	{
		m_Logger = new VkDebugUtilsMessengerEXT;

		// To log ALL information from the Vulkan
		VkDebugUtilsMessengerCreateInfoEXT loggerinfo = {};
		loggerinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		loggerinfo.pfnUserCallback = VulkanLogger::debugCallback;
		loggerinfo.messageSeverity =
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		loggerinfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		loggerinfo.pUserData = nullptr;

		// Setting up messenger
		auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT");
		VkResult result = fn(*instance, &loggerinfo, nullptr, m_Logger);
		VKAPP_ASSERT(result == VK_SUCCESS, "Failed to init messenger!");
	}

	void VulkanLogger::Shutdown(VkInstance* instance)
	{
		auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*instance, "vkDestroyDebugUtilsMessengerEXT");
		fn(*instance, *m_Logger, nullptr);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLogger::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			VKAPP_LOG_TRACE("Vulkan: {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			VKAPP_LOG_INFO("Vulkan: {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			VKAPP_LOG_WARN("Vulkan: {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			VKAPP_LOG_ERROR("Vulkan: {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			break;
		default:
			break;
		}
		return VK_FALSE;
	}

}