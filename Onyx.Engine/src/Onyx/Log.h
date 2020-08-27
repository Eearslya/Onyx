#pragma once

#include "Onyx/Core.h"

namespace Onyx {
class ONYX_API Log {
 public:
  static void Init();

  static void Fatal(const char* msg, ...);
  static void Error(const char* msg, ...);
  static void Warn(const char* msg, ...);
  static void Info(const char* msg, ...);
  static void Debug(const char* msg, ...);
  static void Trace(const char* msg, ...);
};
}  // namespace Onyx

#define OnyxFatal(msg, ...) ::Onyx::Log::Fatal(msg, __VA_ARGS__)
#define OnyxError(msg, ...) ::Onyx::Log::Error(msg, __VA_ARGS__)
#define OnyxWarn(msg, ...) ::Onyx::Log::Warn(msg, __VA_ARGS__)
#define OnyxInfo(msg, ...) ::Onyx::Log::Info(msg, __VA_ARGS__)
#define OnyxDebug(msg, ...) ::Onyx::Log::Debug(msg, __VA_ARGS__)
#define OnyxTrace(msg, ...) ::Onyx::Log::Trace(msg, __VA_ARGS__)