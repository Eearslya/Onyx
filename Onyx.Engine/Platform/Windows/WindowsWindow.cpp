#include "WindowsWindow.h"

namespace Onyx {
const wchar_t* WindowsWindow::ApplicationWindowClass = L"OnyxWindow";

WindowsWindow::~WindowsWindow() {}

const bool WindowsWindow::Initialize(IApplication* application,
                                     const WindowCreateInfo& createInfo) {
  _createInfo = createInfo;
  U32 windowExStyle = 0;
  U32 windowStyle = 0;

  U32 windowX = (U32)createInfo.StartPositionX;
  U32 windowY = (U32)createInfo.StartPositionY;
  U32 windowWidth = (U32)createInfo.StartWidth;
  U32 windowHeight = (U32)createInfo.StartHeight;

  if (createInfo.HasOSWindowBorder) {
    windowExStyle = WS_EX_APPWINDOW;
    windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;

    if (createInfo.IsStandardWindow) {
      if (createInfo.HasMaximize) {
        windowStyle |= WS_MAXIMIZEBOX;
      }
      if (createInfo.HasMinimize) {
        windowStyle |= WS_MINIMIZEBOX;
      }
      if (createInfo.HasSizingFrame) {
        windowStyle |= WS_THICKFRAME;
      } else {
        windowStyle |= WS_BORDER;
      }
    } else {
      windowStyle |= WS_POPUP | WS_BORDER;
    }

    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, false, windowExStyle);

    windowX += borderRect.left;
    windowY += borderRect.top;
    windowWidth += borderRect.right - borderRect.left;
    windowHeight += borderRect.bottom - borderRect.top;
  } else {
    // TODO: Support windows without OS borders
  }

  _handle =
      CreateWindowExW(windowExStyle, ApplicationWindowClass, createInfo.Title,
                      windowStyle, windowX, windowY, windowWidth, windowHeight,
                      nullptr, nullptr, GetModuleHandleW(0), nullptr);
  if (_handle == nullptr) {
    MessageBoxW(nullptr, TEXT("Window creation failed!"), TEXT("Error"),
                MB_ICONEXCLAMATION | MB_OK);
    return false;
  }

  return true;
}

void WindowsWindow::Shutdown() {}

void WindowsWindow::RequestClose() { _closeRequested = true; }

void WindowsWindow::Show() {
  if (!_isVisible) {
    bool shouldActivate = false;
    if (_createInfo.AcceptsInput) {
      shouldActivate = true;
    }

    int showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
    ShowWindow(_handle, showWindowCommandFlags);
    _isVisible = true;
  }
}

void WindowsWindow::Hide() { _isVisible = false; }

const Extent2D WindowsWindow::GetFramebufferExtent() const {
  RECT rect;
  GetWindowRect(_handle, &rect);
  I32 width = rect.right - rect.left;
  I32 height = rect.bottom - rect.top;
  return Extent2D{width, height};
}

WindowsWindow* WindowsWindow::Create() { return new WindowsWindow(); }

WindowsWindow::WindowsWindow() {}
}  // namespace Onyx