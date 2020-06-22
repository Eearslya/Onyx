#pragma once

#include "Defines.h"
#include "Types.h"

namespace Onyx {
class ONYX_API Logger final {
 public:
  static void Trace(const char* msg, ...);
  static void Debug(const char* msg, ...);
  static void Info(const char* msg, ...);
  static void Warn(const char* msg, ...);
  static void Error(const char* msg, ...);
  static void Fatal(const char* msg, ...);
};
}  // namespace Onyx