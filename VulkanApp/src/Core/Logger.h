#pragma once

#include <spdlog/spdlog.h>
#include <memory>

// Logger Class Definition
namespace vkapp {
	class Logger
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; };
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
	};
}

// Log macros definitions are available if Motus is on Debug build.
#ifdef VKAPP_DEBUG
	#define VKAPP_LOG_FATAL(...)		vkapp::Logger::GetCoreLogger()->critical(__VA_ARGS__)
	#define VKAPP_LOG_ERROR(...)		vkapp::Logger::GetCoreLogger()->error(__VA_ARGS__)
	#define VKAPP_LOG_WARN(...)			vkapp::Logger::GetCoreLogger()->warn(__VA_ARGS__)
	#define VKAPP_LOG_INFO(...)			vkapp::Logger::GetCoreLogger()->info(__VA_ARGS__)
	#define VKAPP_LOG_TRACE(...)		vkapp::Logger::GetCoreLogger()->trace(__VA_ARGS__)
// If it is not, macros do basically nothing
#else
	#define VKAPP_LOG_FATAL
	#define VKAPP_LOG_ERROR
	#define VKAPP_LOG_WARN
	#define VKAPP_LOG_INFO
	#define VKAPP_LOG_TRACE
#endif