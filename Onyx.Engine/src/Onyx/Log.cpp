#include "pch.h"

#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstdarg>

namespace Onyx {
static std::shared_ptr<spdlog::logger> g_CoreLogger;

void Log::Init() {
  spdlog::set_pattern("%^[%T] %l: %v%$");
  g_CoreLogger = spdlog::stdout_color_mt("Core");
  g_CoreLogger->set_level(spdlog::level::trace);
}

void Log::Fatal(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  g_CoreLogger->critical(msg, args);
  va_end(args);
}

void Log::Error(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  g_CoreLogger->error(msg, args);
  va_end(args);
}

void Log::Warn(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  g_CoreLogger->warn(msg, args);
  va_end(args);
}

void Log::Info(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  g_CoreLogger->info(msg, args);
  va_end(args);
}

void Log::Debug(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  g_CoreLogger->debug(msg, args);
  va_end(args);
}

void Log::Trace(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  g_CoreLogger->trace(msg, args);
  va_end(args);
}
}  // namespace Onyx