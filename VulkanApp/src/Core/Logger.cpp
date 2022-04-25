#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace vkapp {
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;

	void Logger::Init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		
		s_CoreLogger = spdlog::stdout_color_mt("VulkanApp");

		s_CoreLogger->set_level(spdlog::level::trace);

		VKAPP_LOG_INFO("Logger inialized successfully.");

	}
}