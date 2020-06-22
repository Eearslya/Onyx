#pragma once

#include "Defines.h"
#include "Types.h"

#include <windows.h>

#include "Platform/IWindow.h"

namespace Onyx {
class IApplication;

class WindowsWindow final : public IWindow {
 public:
  ~WindowsWindow();

  void Initialize(IApplication* application,
                  const WindowCreateInfo& createInfo) override;
  void RequestClose() override;
  void Show() override;
  void Hide() override;

  const bool IsCloseRequested() const override { return _closeRequested; }
  const Extent2D GetFramebufferExtent() const override;
  void* GetHandle() override { return _handle; }

  static WindowsWindow* Create();
  static const wchar_t* ApplicationWindowClass;

 private:
  WindowsWindow();

  bool _isVisible = false;
  bool _closeRequested = false;
  WindowCreateInfo _createInfo;
  HWND _handle;
};
}  // namespace Onyx