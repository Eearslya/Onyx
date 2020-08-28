#include "pch.h"

#include "Window.h"

#ifdef ONYX_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Onyx {
Scope<Window> Window::Create(const WindowProps& props) {
#ifdef ONYX_PLATFORM_WINDOWS
  return CreateScope<WindowsWindow>(props);
#endif
}
}  // namespace Onyx