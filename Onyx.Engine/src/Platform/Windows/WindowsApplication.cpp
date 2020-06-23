#include "pch.h"

#include "WindowsApplication.h"

namespace Onyx {
#ifdef ONYX_PLATFORM_WINDOWS
IApplication* Application::CreateApplication(const wchar_t* applicationName) {
  WindowsApplication* app = new WindowsApplication();
  return app;
}
#endif

WindowsApplication::WindowsApplication() {}

WindowsApplication::~WindowsApplication() {}

void WindowsApplication::Run() {}
}  // namespace Onyx