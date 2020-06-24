#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <Windows.h>

#include "Platform/IWindow.h"

namespace Onyx {
class WindowsApplication;

class WindowsWindow final : public IWindow {
 public:
  WindowsWindow(WindowsApplication* application, const WindowCreateInfo& createInfo);
  ~WindowsWindow();

  void ProcessMessages(const F32 deltaTime);
  LRESULT ProcessMessage(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam);

  void Show() override;
  void Hide() override;
  void RequestClose() override;

  bool CloseRequested() override { return _closeRequested; }
  void* GetHandle() override { return _hwnd; }

 private:
  void RegisterWindowClass(HINSTANCE instance);

  WindowsApplication* _application = nullptr;
  HWND _hwnd = nullptr;
  bool _visible = false;
  bool _closeRequested = false;

  static bool _classRegistered;
};
}  // namespace Onyx