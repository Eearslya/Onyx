#pragma once

#include "Defines.h"
#include "Types.h"

#include <vector>

namespace Onyx {
class Renderer final {
 public:
  static const bool Initialize();
  static void Shutdown();
  static const bool PrepareFrame();
  static const bool Frame();

 private:
  static const bool CreateInstance();

  static void DestroyInstance();

  static const bool ValidateInstanceExtensions(const std::vector<const char*> extensions);
};
}  // namespace Onyx