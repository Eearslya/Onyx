#pragma once

#include <spdlog/spdlog.h>
//
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

#include "Onyx/Core.h"

namespace Onyx {
extern std::shared_ptr<spdlog::logger> g_CoreLogger;

class ONYX_API Log {
 public:
  static void Init();
  static Ref<spdlog::logger>& GetLogger();
};
}  // namespace Onyx

#define OnyxFatal(msg, ...) ::Onyx::Log::GetLogger()->critical(msg, __VA_ARGS__)
#define OnyxError(msg, ...) ::Onyx::Log::GetLogger()->error(msg, __VA_ARGS__)
#define OnyxWarn(msg, ...) ::Onyx::Log::GetLogger()->warn(msg, __VA_ARGS__)
#define OnyxInfo(msg, ...) ::Onyx::Log::GetLogger()->info(msg, __VA_ARGS__)
#define OnyxDebug(msg, ...) ::Onyx::Log::GetLogger()->debug(msg, __VA_ARGS__)
#define OnyxTrace(msg, ...) ::Onyx::Log::GetLogger()->trace(msg, __VA_ARGS__)

#define OnyxAssert(expr, ...)                                                           \
  {                                                                                     \
    if (!(expr)) {                                                                      \
      OnyxFatal("--- Assertion Failed ---\n  @{}:{}\n  {}", __FILE__, __LINE__, #expr); \
      __debugbreak();                                                                   \
    }                                                                                   \
  }