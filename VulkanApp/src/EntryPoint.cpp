#include <Core/Application.h>
#include <Core/Logger.h>

int main() {
	vkapp::Logger::Init();

	vkapp::Application* app = new vkapp::Application;
	app->Run();
	delete app;

}