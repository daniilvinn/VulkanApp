#include "ApplicationWindow.h"

#include <Core/Logger.h>

namespace vkapp {

	static void glfwErrorCallback(int code, const char* error) {
		VKAPP_LOG_FATAL("GLFW_ERROR: {0} ({1}) ", error, code);
	}

	ApplicationWindow::ApplicationWindow(const ApplicationWindowProperties& props)
		: m_Props(props)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_WindowHandle = glfwCreateWindow(
			props.width,
			props.height,
			props.title,
			NULL, 
			NULL
		);
		
		glfwSetErrorCallback(glfwErrorCallback);

		m_Context = new VulkanContext(m_WindowHandle);
		m_Context->Init();
	}

	ApplicationWindow::~ApplicationWindow()
	{
		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
		delete m_Context;
	}
}