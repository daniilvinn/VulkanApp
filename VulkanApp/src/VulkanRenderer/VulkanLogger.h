#pragma once

#include <vulkan/vulkan.h>

namespace vkapp {
	class VulkanLogger
	{
	public:
		static void Init(VkInstance* instance);
		static void Shutdown(VkInstance* instance);

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		static VkDebugUtilsMessengerEXT* m_Logger;
	};
}