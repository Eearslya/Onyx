#include "Logger.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>

namespace Onyx {
static const char* LogLevelNames[] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
static char LogBuffer[8192];

#ifdef ONYX_TRACE
Logger::LogLevel Logger::_logLevel = Logger::LogLevel::Trace;
#elif defined(ONYX_DEBUG)
Logger::LogLevel Logger::_logLevel = Logger::LogLevel::Debug;
#else
Logger::LogLevel Logger::_logLevel = Logger::LogLevel::Info;
#endif

void Logger::WriteLog(Logger::LogLevel level, const char* message, va_list args) {
  if (level > _logLevel) {
    return;
  }
  vsnprintf(LogBuffer, sizeof(LogBuffer), message, args);
  printf("[%s]: %s\n", LogLevelNames[static_cast<U32>(level)], LogBuffer);
}

void Logger::Trace(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(LogLevel::Trace, msg, args);
  va_end(args);
}

void Logger::Debug(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(LogLevel::Debug, msg, args);
  va_end(args);
}

void Logger::Info(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(LogLevel::Info, msg, args);
  va_end(args);
}

void Logger::Warn(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(LogLevel::Warn, msg, args);
  va_end(args);
}

void Logger::Error(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(LogLevel::Error, msg, args);
  va_end(args);
}

void Logger::Fatal(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(LogLevel::Fatal, msg, args);
  va_end(args);
}
}  // namespace Onyx