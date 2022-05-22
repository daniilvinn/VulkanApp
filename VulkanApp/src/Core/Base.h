#pragma once
#include <Core/Logger.h>

#ifdef VKAPP_DEBUG
	#define VKAPP_ENABLE_VULKAN_VALIDATION_LAYERS 1
	#define VKAPP_ENABLE_VULKAN_MESSENGER 1
	#ifndef VKAPP_ASSERTS_ENABLED
		#define VKAPP_ASSERTS_ENABLED
		#ifdef VKAPP_PLATFORM_WINDOWS
			#define VKAPP_ASSERT(expression, ...)   if(!(expression)){\
													VKAPP_LOG_ERROR("Assertion failed: {}",__VA_ARGS__); __debugbreak();}\
													else{}
		#endif
	#endif
#else
#define VKAPP_ENABLE_VULKAN_VALIDATION_LAYERS 0
#define VKAPP_ENABLE_VULKAN_MESSENGER 0
	#define VKAPP_ASSERT(expression, ...)
#endif

#define VK_CHECK_RESULT(fn) { \
							VkResult result = fn;\
							if(result != VK_SUCCESS) { \
								VKAPP_LOG_FATAL("Vulkan call failed: {0} ({1})", __FILE__, __LINE__);\
								throw std::runtime_error("Vulkan call failed!");\
							} \
							}

// Thanks Yan TheCherno Chernikov for this macro <3
// https://github.com/TheCherno
#define VKAPP_BIND_EVENT_FUNCTION(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
// --------------------------------------

#include <memory>

namespace vkapp {
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}