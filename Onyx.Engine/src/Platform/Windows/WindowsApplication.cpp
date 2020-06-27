#include "pch.h"

#include "Platform/Application.h"

namespace Onyx {
#ifdef ONYX_PLATFORM_WINDOWS
#include <Windows.h>

// Generic application variables.
const wchar_t* Application::s_ApplicationName;
bool Application::s_WindowVisible = false;
bool Application::s_CloseRequested = false;
Application Application::s_Application;

// Platform-specific variables.
static HINSTANCE s_hInstance;
static HWND s_hWnd;
static const wchar_t* s_WindowClassName = L"OnyxWindow";

LRESULT CALLBACK ApplicationWindowProcedure(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_ERASEBKGND:
      return 1;
    case WM_CLOSE:
      Logger::Trace("[Window] WM_CLOSE");
      Application::RequestClose();
      return 0;
    case WM_DESTROY:
      Logger::Trace("[Window] WM_DESTROY");
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

const bool Application::Initialize(const wchar_t* applicationName) {
  Logger::Info("Initializing Windows application...");
  s_hInstance = GetModuleHandleW(nullptr);

  if (!CreateApplicationWindow()) {
    return false;
  }

  ShowApplicationWindow();
  
  if (!Engine::Initialize()) {
    return false;
  }

  return true;
}

void Application::Shutdown() {
  Engine::Shutdown();
  DestroyApplicationWindow();
}

const bool Application::CreateApplicationWindow() {
  WNDCLASSW wc{};
  wc.lpfnWndProc = ApplicationWindowProcedure;
  wc.hInstance = s_hInstance;
  wc.lpszClassName = s_WindowClassName;
  RegisterClassW(&wc);

  // Set up some default window styling.
  U32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME;
  U32 windowExStyle = WS_EX_APPWINDOW;
  // windowStyle |= WS_MAXIMIZEBOX;
  // windowStyle |= WS_MINIMIZEBOX;

  // Determine how big to make our actual window.
  // This will ensure that the drawable space is made at the requested size.
  // TODO: Somehow make this configurable.
  U32 windowW = 1600;
  U32 windowH = 900;
  U32 windowX = (1920 - windowW) / 2;
  U32 windowY = (1080 - windowH) / 2;

  // Calculate how thick the window borders will be, and stretch the window
  // accordingly, such that our draw space matches the space requested.
  RECT borderRect = {0, 0, 0, 0};
  AdjustWindowRectEx(&borderRect, windowStyle, false, windowExStyle);
  windowX += borderRect.left;
  windowY += borderRect.top;
  windowW += borderRect.right - borderRect.left;
  windowH += borderRect.bottom - borderRect.top;

  s_hWnd =
      CreateWindowExW(windowExStyle, s_WindowClassName, s_ApplicationName, windowStyle, windowX,
                      windowY, windowW, windowH, nullptr, nullptr, s_hInstance, nullptr);

  if (s_hWnd == nullptr) {
    DWORD errorCode = GetLastError();
    Logger::Fatal("Failed to create application window!");
    MessageBoxW(NULL, TEXT("Window creation failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
    return false;
  }

  return true;
}

void Application::ShowApplicationWindow() {
  if (!s_WindowVisible) {
    ShowWindow(s_hWnd, SW_SHOW);
    s_WindowVisible = true;
  }
}

void Application::HideApplicationWindow() {
  if (s_WindowVisible) {
    s_WindowVisible = false;
  }
}

void Application::DestroyApplicationWindow() { DestroyWindow(s_hWnd); }

void Application::Run() {
  while (!s_CloseRequested) {
    ProcessEvents();

    Engine::OnLoop();
  }
}

void Application::RequestClose() { s_CloseRequested = true; }

void Application::ProcessEvents() {
  MSG message;
  while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
}

void* Application::GetHandle() { return static_cast<void*>(s_hInstance); }

void* Application::GetWindowHandle() { return static_cast<void*>(s_hWnd); }

Extent2D Application::GetWindowExtent() {
  RECT windowRect;
  GetWindowRect(s_hWnd, &windowRect);
  U32 width = windowRect.right - windowRect.left;
  U32 height = windowRect.bottom - windowRect.top;
  return {width, height};
}
#endif
}  // namespace Onyx