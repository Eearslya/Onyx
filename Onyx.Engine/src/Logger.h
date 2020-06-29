#pragma once

#include "Defines.h"
#include "Types.h"

namespace Onyx {
class ONYX_API Logger final {
 public:
  enum class LogLevel { Fatal = 0, Error, Warn, Info, Debug, Trace };

  static void SetLevel(LogLevel newLevel) { _logLevel = newLevel; }

  static void Trace(const char* msg, ...);
  static void Debug(const char* msg, ...);
  static void Info(const char* msg, ...);
  static void Warn(const char* msg, ...);
  static void Error(const char* msg, ...);
  static void Fatal(const char* msg, ...);

 private:
  static void WriteLog(LogLevel level, const char* msg, va_list args);

  static LogLevel _logLevel;
};
}  // namespace Onyx