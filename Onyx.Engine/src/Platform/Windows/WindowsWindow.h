#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <Windows.h>

#include "Platform/IWindow.h"

namespace Onyx {
namespace Platform {
class WindowsApplication;

//! IWindow implementation for Windows.
class WindowsWindow final : public IWindow {
 public:
  //! Construct a WindowsWindow.
  /*!
    \param application The application this window belongs to.
    \param createInfo Various parameters to control window creation.
  */
  WindowsWindow(WindowsApplication* application, const WindowCreateInfo& createInfo);
  ~WindowsWindow();

  //! Process all window messages and dispatch them accordingly.
  void ProcessMessages();

  //! Handle a window message.
  /*!
    \param hwnd The native window handle.
    \param msg The message type.
    \param wParam The message's WPARAM.
    \param lParam The message's LPARAM.
  */
  LRESULT ProcessMessage(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam);

  void Show() override;
  void Hide() override;
  void RequestClose() override;

  bool CloseRequested() override { return _closeRequested; }
  void* GetHandle() override { return _hwnd; }
  Extent2D GetExtent() override;

 private:
  void RegisterWindowClass(HINSTANCE instance);

  WindowsApplication* _application = nullptr;
  HWND _hwnd = nullptr;
  bool _visible = false;
  bool _closeRequested = false;

  static bool _classRegistered;
};
}  // namespace Platform
}  // namespace Onyx