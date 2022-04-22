#include "Application.h"
#include <iostream>

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
	while (true)
	{
		std::cout << "Hello World!" << std::endl;
	}
}

void Application::Shutdown()
{

}

void Application::Initialize()
{

}
