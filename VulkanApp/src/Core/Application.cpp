#include "Application.h"
#include "Logger.h"
#include <Core/Base.h>

#include <glfw/glfw3.h>

#include <VulkanRenderer/VulkanPipeline.h>
#include <VulkanRenderer/VulkanRenderer.h>
#include <VulkanRenderer/VulkanContext.h>

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
		VulkanPipeline pipeline;

		auto swapchain = VulkanContext::Get()->GetSwapchain();
		

		while (!glfwWindowShouldClose(m_Window->GetHandle()))
		{
			
			swapchain->BeginFrame();

			VulkanRenderer::BeginRenderPass(swapchain->GetRenderPass(), swapchain->GetCurrentFramebuffer());
			VulkanRenderer::Submit(pipeline);
			VulkanRenderer::EndRenderPass();

			swapchain->Present();
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