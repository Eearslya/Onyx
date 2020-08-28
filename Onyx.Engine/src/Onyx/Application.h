#pragma once

#include "Onyx/Core.h"
#include "Onyx/Window.h"

namespace Onyx {
class Application {
 public:
  ONYX_API Application();
  ONYX_API virtual ~Application();

  ONYX_API void Run();

 private:
  Scope<Window> m_Window;
  bool m_Running = false;
};

Application* CreateApplication();
}  // namespace Onyx