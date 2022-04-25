#pragma once

#include "ApplicationWindow.h"

namespace vkapp {
	class Application
	{
	public:
		Application();
		~Application();

		void Run();
		inline static Application* Get() { return s_AppInstance; };

	private:
		void Shutdown();
		void Initialize();

	private:
		static Application* s_AppInstance;
		ApplicationWindow* m_Window;
	};
}