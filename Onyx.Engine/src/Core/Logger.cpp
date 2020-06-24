#include "pch.h"

#include "Logger.h"

#include <stdarg.h>
#include <stdio.h>

namespace Onyx {
static const char* LogLevelNames[] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
static char LogBuffer[8192];

#ifdef ONYX_TRACE
Logger::Level Logger::_logLevel = Logger::Level::TRACE;
#elif defined(ONYX_DEBUG)
Logger::Level Logger::_logLevel = Logger::Level::DEBUG;
#else
Logger::Level Logger::_logLevel = Logger::Level::INFO;
#endif

void Logger::WriteLog(Logger::Level level, const char* message, va_list args) {
  if (level > _logLevel) {
    return;
  }
  vsnprintf(LogBuffer, sizeof(LogBuffer), message, args);
  printf("[%s]: %s\n", LogLevelNames[static_cast<U32>(level)], LogBuffer);
}

void Logger::Trace(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(Level::TRACE, msg, args);
  va_end(args);
}

void Logger::Debug(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(Level::DEBUG, msg, args);
  va_end(args);
}

void Logger::Info(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(Level::INFO, msg, args);
  va_end(args);
}

void Logger::Warn(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(Level::WARN, msg, args);
  va_end(args);
}

void Logger::Error(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(Level::ERROR, msg, args);
  va_end(args);
}

void Logger::Fatal(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog(Level::FATAL, msg, args);
  va_end(args);
}
}  // namespace Onyx