#include "Logger.h"

#include <stdarg.h>
#include <stdio.h>

#include <iostream>

namespace Onyx {
static void WriteLog(const char* prefix, const char* message, va_list args) {
  char buffer[8192];
  vsnprintf(buffer, 8192, message, args);
  std::cout << "[" << prefix << "]: " << buffer << "\n";
}

void Logger::Trace(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog("TRACE", msg, args);
  va_end(args);
}

void Logger::Debug(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog("DEBUG", msg, args);
  va_end(args);
}

void Logger::Info(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog("INFO", msg, args);
  va_end(args);
}

void Logger::Warn(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog("WARN", msg, args);
  va_end(args);
}

void Logger::Error(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog("ERROR", msg, args);
  va_end(args);
}

void Logger::Fatal(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  WriteLog("FATAL", msg, args);
  va_end(args);
}
}  // namespace Onyx