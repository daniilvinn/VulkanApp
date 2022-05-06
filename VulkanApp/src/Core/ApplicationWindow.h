#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VulkanRenderer/VulkanContext.h>

#include <utility>

namespace vkapp {

	
	struct ApplicationWindowProperties {
		int32_t width;
		int32_t height;
		const char* title;

		ApplicationWindowProperties()
			: width(1600), height(900), title("Vulkan App")
		{}
	};
		
	class ApplicationWindow {
	public:
		ApplicationWindow(const ApplicationWindowProperties& props = ApplicationWindowProperties());
		~ApplicationWindow();

		inline GLFWwindow* GetHandle() { return m_WindowHandle; };
		std::pair<int32_t, int32_t> GetSize() { return { m_Props.width, m_Props.height }; };

	private:
		GLFWwindow* m_WindowHandle;
		VulkanContext* m_Context;

		ApplicationWindowProperties m_Props;

	};

}