#include "pch.h"

#include "WindowsApplication.h"

#include "Platform/IWindow.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Onyx {
#ifdef ONYX_PLATFORM_WINDOWS
Platform::IApplication* Application::CreateApplication(const wchar_t* applicationName) {
  Platform::WindowsApplication* app = new Platform::WindowsApplication(applicationName);
  return app;
}

void Application::DestroyApplication(Platform::IApplication* app) { delete app; }
#endif

namespace Platform {
WindowsApplication::WindowsApplication(const wchar_t* applicationName)
    : IApplication(applicationName) {
  _hInstance = GetModuleHandleW(nullptr);

  WindowCreateInfo createInfo{};
  createInfo.StartPositionX = 100;
  createInfo.StartPositionY = 100;
  createInfo.StartWidth = 1280;
  createInfo.StartHeight = 720;
  createInfo.AllowMaximize = true;
  createInfo.AllowMinimize = true;
  createInfo.Title = applicationName;

  _mainWindow = static_cast<WindowsWindow*>(Window::CreateApplicationWindow(this, createInfo));
  _mainWindow->Show();
}

WindowsApplication::~WindowsApplication() {}

void WindowsApplication::Run() {
  while (!_mainWindow->CloseRequested()) {
    _mainWindow->ProcessMessages();
  }
}
}  // namespace Platform
}  // namespace Onyx