#include "pch.h"

#include "Log.h"

namespace Onyx {
static Ref<spdlog::logger> s_Logger;

void Log::Init() {
  spdlog::set_pattern("%^[%T] %l: %v%$");
  s_Logger = spdlog::stdout_color_mt("Core");
  s_Logger->set_level(spdlog::level::trace);
}

Ref<spdlog::logger>& Log::GetLogger() { return s_Logger; }
}  // namespace Onyx