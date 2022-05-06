#pragma once

#include "ApplicationWindow.h"

namespace vkapp {
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		static Application* Get() { return s_AppInstance; }
		inline ApplicationWindow* GetWindow() { return m_Window; }

	private:
		void Shutdown();
		void Initialize();

	private:
		static Application* s_AppInstance;
		ApplicationWindow* m_Window;
	};
}