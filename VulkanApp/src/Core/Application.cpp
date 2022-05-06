#include "Application.h"
#include "Logger.h"
#include <Core/Base.h>

#include <glfw/glfw3.h>

namespace vkapp {
	Application* Application::s_AppInstance = nullptr;

	Application::Application()
	{
		Initialize();
	}

	Application::~Application()
	{
		Shutdown();
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window->GetHandle()))
		{
			glfwPollEvents();
		}
	}

	void Application::Shutdown()
	{
		delete m_Window;
	}

	void Application::Initialize()
	{
		VKAPP_ASSERT(!s_AppInstance, "Application already exists!");
		s_AppInstance = this;
		m_Window = new ApplicationWindow();
		
	}
}