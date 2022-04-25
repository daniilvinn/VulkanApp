#pragma once

#include <GLFW/glfw3.h>
#include <VulkanRenderer/VulkanContext.h>

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

		inline GLFWwindow* GetHandle() { m_WindowHandle; };

	private:
		GLFWwindow* m_WindowHandle;
		VulkanContext* m_Context;
	};

}