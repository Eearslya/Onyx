#include "pch.h"

#include "WindowsWindow.h"

#include "Core/Defines.h"
#include "Core/Types.h"

#include "Platform/IWindow.h"
#include "Platform/Windows/WindowsApplication.h"

namespace Onyx {
#ifdef ONYX_PLATFORM_WINDOWS
Platform::IWindow* Window::CreateApplicationWindow(Platform::IApplication* application,
                                                   const Platform::WindowCreateInfo& createInfo) {
  Platform::WindowsWindow* window = new Platform::WindowsWindow(
      static_cast<Platform::WindowsApplication*>(application), createInfo);
  return window;
}

void Window::DestroyApplicationWindow(Platform::IWindow* window) { delete window; }
#endif

namespace Platform {

//! Indicates whether we've registered our window class.
bool WindowsWindow::_classRegistered = false;

//! Name for our window class.
const static wchar_t* WindowClassName = L"OnyxWindow";

//! Our main window. Temporary global variable.
// TODO: Allow for multiple windows. Requires more complex dispatch inside of
// ApplicationWindowProcedure.
WindowsWindow* MainWindow;

//! Windows callback for message handling.
LRESULT CALLBACK ApplicationWindowProcedure(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam) {
  return MainWindow->ProcessMessage(hwnd, msg, wParam, lParam);
}

WindowsWindow::WindowsWindow(WindowsApplication* application, const WindowCreateInfo& createInfo)
    : IWindow(application, createInfo), _application(application) {
  // TODO: Keep a list of windows
  MainWindow = this;

  HINSTANCE appInstance = static_cast<HINSTANCE>(_application->GetHandle());
  RegisterWindowClass(appInstance);

  // Set up some default window styling.
  // TODO: Make some of this customizable with CreateInfo.
  U32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME;
  U32 windowExStyle = WS_EX_APPWINDOW;
  windowStyle |= createInfo.AllowMaximize ? WS_MAXIMIZEBOX : 0;
  windowStyle |= createInfo.AllowMinimize ? WS_MINIMIZEBOX : 0;

  // Determine how big to make our actual window.
  // This will ensure that the drawable space is made at the requested size.
  U32 windowX = createInfo.StartPositionX;
  U32 windowY = createInfo.StartPositionY;
  U32 windowW = createInfo.StartWidth;
  U32 windowH = createInfo.StartHeight;

  // Calculate how thick the window borders will be, and stretch the window
  // accordingly, such that our draw space matches the space requested.
  RECT borderRect = {0, 0, 0, 0};
  AdjustWindowRectEx(&borderRect, windowStyle, false, windowExStyle);
  windowX += borderRect.left;
  windowY += borderRect.top;
  windowW += borderRect.right - borderRect.left;
  windowH += borderRect.bottom - borderRect.top;

  _hwnd = CreateWindowExW(windowExStyle, WindowClassName, createInfo.Title, windowStyle, windowX,
                          windowY, windowW, windowH, nullptr, nullptr, appInstance, nullptr);

  if (_hwnd == nullptr) {
    DWORD errorCode = GetLastError();
    MessageBoxW(NULL, TEXT("Window creation failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
  }
}

WindowsWindow::~WindowsWindow() {}

void WindowsWindow::ProcessMessages() {
  MSG message;
  while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
}

LRESULT WindowsWindow::ProcessMessage(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_ERASEBKGND:
      return 1;
    case WM_CLOSE:
      RequestClose();
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void WindowsWindow::Show() {
  if (!_visible) {
    ShowWindow(_hwnd, SW_SHOW);
    _visible = true;
  }
}

void WindowsWindow::Hide() {
  if (_visible) {
    _visible = false;
  }
}

void WindowsWindow::RequestClose() { _closeRequested = true; }

Extent2D WindowsWindow::GetExtent() {
  RECT windowRect;
  GetWindowRect(_hwnd, &windowRect);
  U32 width = windowRect.right - windowRect.left;
  U32 height = windowRect.bottom - windowRect.top;
  return {width, height};
}

void WindowsWindow::RegisterWindowClass(HINSTANCE instance) {
  if (_classRegistered) {
    return;
  }

  WNDCLASSW wc{};
  wc.lpfnWndProc = ApplicationWindowProcedure;
  wc.hInstance = instance;
  wc.lpszClassName = WindowClassName;
  RegisterClassW(&wc);

  _classRegistered = true;
}
}  // namespace Platform
}  // namespace Onyx