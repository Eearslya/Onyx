#include "Application.h"

#include "Logger.h"
#include "Renderer.h"

namespace Onyx {
HINSTANCE Application::s_Instance;
HWND Application::s_Window;
bool Application::s_CloseRequested = false;

static const U32 g_WindowWidth = 1280;
static const U32 g_WindowHeight = 720;
static const wchar_t* g_WindowClassName = L"OnyxWindow";

LRESULT CALLBACK ApplicationWindowProcedure(HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_ERASEBKGND:
      return 1;
    case WM_CLOSE:
      Application::RequestClose();
      return 0;
    case WM_DESTROY:
      Logger::Fatal("Application window destroyed. Shutting down...");
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

const bool Application::Initialize() {
  Logger::Info("Initializing Onyx...");
  WNDCLASSW wc{};
  wc.lpfnWndProc = ApplicationWindowProcedure;
  wc.hInstance = s_Instance;
  wc.lpszClassName = g_WindowClassName;
  RegisterClassW(&wc);

  U32 windowStyle =
      WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
  U32 windowExStyle = WS_EX_APPWINDOW;

  U32 windowW = g_WindowWidth;
  U32 windowH = g_WindowHeight;
  U32 windowX = 100;
  U32 windowY = 100;

  RECT borderRect = {0, 0, 0, 0};
  AdjustWindowRectEx(&borderRect, windowStyle, false, windowExStyle);
  windowX += borderRect.left;
  windowY += borderRect.top;
  windowW += borderRect.right - borderRect.left;
  windowH += borderRect.bottom - borderRect.top;

  Logger::Trace("Creating %dx%d application window. (Actual size: %dx%d)", g_WindowWidth,
                g_WindowHeight, windowW, windowH);

  s_Window = CreateWindowExW(windowExStyle, g_WindowClassName, L"Onyx", windowStyle, windowX,
                             windowY, windowW, windowH, nullptr, nullptr, s_Instance, nullptr);
  if (!s_Window) {
    Logger::Fatal("Failed to create application window!");
    ASSERT(false);
  }

  ShowWindow(s_Window, SW_SHOW);

  if (!Renderer::Initialize()) {
    Logger::Fatal("Failed to initialize application renderer!");
    ASSERT(false);
  }

  return true;
}

void Application::Shutdown() { Renderer::Shutdown(); }

void Application::Run() {
  Logger::Trace("Beginning main application loop.");
  while (!s_CloseRequested) {
    ProcessEvents();

    if (!Renderer::Frame()) {
      Logger::Error("Error during frame draw!");
    }
  }
  Logger::Trace("Main application loop ended.");
}

Extent2D Application::GetWindowExtent() {
  RECT windowRect;
  GetWindowRect(s_Window, &windowRect);
  return {static_cast<U32>(windowRect.right) - static_cast<U32>(windowRect.left),
          static_cast<U32>(windowRect.bottom) - static_cast<U32>(windowRect.top)};
}

void Application::ProcessEvents() {
  MSG message;
  while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
}

void Application::RequestClose() {
  Logger::Info("Program close requested.");
  s_CloseRequested = true;
}
}  // namespace Onyx