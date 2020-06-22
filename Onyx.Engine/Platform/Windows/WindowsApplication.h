#pragma once

#include "Defines.h"
#include "Types.h"

#include <windows.h>

#include "Input/Input.h"
#include "Platform/IApplication.h"

namespace Onyx {
class Engine;
class WindowsWindow;

class WindowsApplication final : public IApplication {
 public:
  ~WindowsApplication();

  const bool Initialize() override;
  void Shutdown() override;

  void Run() override;
  void PumpMessages(const F32 deltaTime) override;
  I32 ProcessMessage(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam);
  IWindow* CreateApplicationWindow(const WindowCreateInfo& createInfo) override;
  IWindow* GetApplicationWindow() override;

  static WindowsApplication* CreateWindowsApplication(const HINSTANCE handle,
                                                      const HICON iconHandle);

 private:
  WindowsApplication(const HINSTANCE handle, const HICON iconHandle);
  void HandleKeyDown(Key key);
  void HandleKeyUp(Key key);
  const bool RegisterWindowClass(const HINSTANCE handle, const HICON iconHandle);

  Engine* _engine;
  HINSTANCE _handle;
  WindowsWindow* _mainWindow;
};
}  // namespace Onyx