#pragma once

#include "Onyx/Core.h"

namespace Onyx {
class ONYX_API Application {
 public:
  Application();
  virtual ~Application();

  void Run();
};

Application* CreateApplication();
}  // namespace Onyx