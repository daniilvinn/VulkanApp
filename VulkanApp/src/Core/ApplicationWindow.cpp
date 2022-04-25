#include "ApplicationWindow.h"

namespace vkapp {
	ApplicationWindow::ApplicationWindow(const ApplicationWindowProperties& props)
	{
		glfwInit();
		m_WindowHandle = glfwCreateWindow(
			props.width,
			props.height,
			props.title,
			NULL, 
			NULL
		);

		m_Context = new VulkanContext(m_WindowHandle);
		m_Context->Init();
	}

	ApplicationWindow::~ApplicationWindow()
	{
		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
	}
}