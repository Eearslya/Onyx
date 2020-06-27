#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
class ONYX_API Engine final {
 public:
  static void Initialize();
  static void Shutdown();

  static void Run();
  static const bool OnLoop();
};
}  // namespace Onyx