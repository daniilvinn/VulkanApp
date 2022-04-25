#include "Application.h"
#include "Logger.h"

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

		m_Window = new ApplicationWindow();
		while (true)
		{

		}
	}

	void Application::Shutdown()
	{
		
	}

	void Application::Initialize()
	{
		if (s_AppInstance) VKAPP_LOG_ERROR("Application already exists!");
		s_AppInstance = this;
	}
}