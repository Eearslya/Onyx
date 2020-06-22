#include "WindowsApplication.h"

#include <Windows.h>

#include "Engine.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Onyx {
WindowsApplication* App;

LRESULT CALLBACK ApplicationWindowProcedure(HWND hwnd, U32 msg, WPARAM wParam,
                                            LPARAM lParam) {
  return App->ProcessMessage(hwnd, msg, wParam, lParam);
}

WindowsApplication* WindowsApplication::CreateWindowsApplication(
    const HINSTANCE handle, const HICON iconHandle) {
  App = new WindowsApplication(handle, iconHandle);
  return App;
}

WindowsApplication::WindowsApplication(const HINSTANCE handle,
                                       const HICON iconHandle) {
  _handle = handle;
  const bool classRegistered = RegisterWindowClass(handle, iconHandle);
  // TODO: Setup input
}

void WindowsApplication::HandleKeyDown(Key key) {
  // TODO: Post key event
}

void WindowsApplication::HandleKeyUp(Key key) {
  // TODO: Post key event
}

const bool WindowsApplication::RegisterWindowClass(const HINSTANCE handle,
                                                   const HICON iconHandle) {
  WNDCLASSW wc{};
  wc.style = CS_DBLCLKS;
  wc.lpfnWndProc = ApplicationWindowProcedure;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = handle;
  wc.hIcon = iconHandle;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = NULL;
  wc.lpszClassName = WindowsWindow::ApplicationWindowClass;

  if (!RegisterClassW(&wc)) {
    MessageBoxW(nullptr, TEXT("Window registration failed!"), TEXT("Error"),
                MB_ICONEXCLAMATION | MB_OK);
    return false;
  }

  return true;
}

WindowsApplication::~WindowsApplication() {
  if (_engine) {
    delete _engine;
  }
  if (_mainWindow) {
    delete _mainWindow;
  }
}

const bool WindowsApplication::Initialize() {
  _engine = new Engine(this);
  return true;
}

void WindowsApplication::Shutdown() {
  if (_engine) {
    _engine->Shutdown();
  }
  if (_mainWindow) {
    _mainWindow->Shutdown();
  }
}

void WindowsApplication::Run() {
  // TODO: Clock
  //_clock = new Clock(true);
  if (_engine) {
    _engine->Run();
  }
  //_lastTime = _clock.GetTime();
  while (!_mainWindow->IsCloseRequested()) {
    // U64 currentTime = _clock.GetTime();
    // F32 deltaTime = (F32)(currentTime - _lastTime);
    // deltaTime *= 0.001f;
    F32 deltaTime = 0.0f;

    PumpMessages(deltaTime);

    if (!_mainWindow->IsCloseRequested()) {
      if (!_engine->OnLoop(deltaTime)) {
        // Engine failed to run!
      }

      //_lastTime = currentTime;
    }
  }

  PostMessageW((HWND)_mainWindow->GetHandle(), WM_DESTROY, 0, 0);
}

void WindowsApplication::PumpMessages(const F32 deltaTime) {
  MSG message;
  while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
}

I32 WindowsApplication::ProcessMessage(HWND hwnd, U32 msg, WPARAM wParam,
                                       LPARAM lParam) {
  switch (msg) {
    case WM_ERASEBKGND: {
      return 1;
    }
    case WM_CLOSE: {
      _mainWindow->RequestClose();
      return 0;
    }
    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    }
    case WM_SIZING: {
      break;
    }
    case WM_SIZE: {
      U16 width = LOWORD(lParam);
      U16 height = HIWORD(lParam);

      // TODO: Send resize event
      break;
    }
    case WM_KEYDOWN: {
      HandleKeyDown((Key)wParam);
      break;
    }
    case WM_KEYUP: {
      HandleKeyUp((Key)wParam);
      break;
    }
  }
  return (I32)DefWindowProcW(hwnd, msg, wParam, lParam);
}

IWindow* WindowsApplication::CreateApplicationWindow(
    const WindowCreateInfo& createInfo) {
  _mainWindow = WindowsWindow::Create();
  _mainWindow->Initialize(this, createInfo);
  return _mainWindow;
}

IWindow* WindowsApplication::GetApplicationWindow() {
  return static_cast<IWindow*>(_mainWindow);
}

IApplication *Application::CreateApplication(const wchar_t *applicationName) {
  HICON icon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);
  WindowsApplication* app =
      WindowsApplication::CreateWindowsApplication(GetModuleHandleW(0), icon);
  app->Initialize();

  WindowCreateInfo createInfo{};
  createInfo.StartPositionX = 100.0f;
  createInfo.StartPositionY = 100.0f;
  createInfo.StartWidth = 1280.0f;
  createInfo.StartHeight = 720.0f;
  createInfo.AcceptsInput = true;
  createInfo.IsStandardWindow = true;
  createInfo.HasOSWindowBorder = true;
  createInfo.HasCloseButton = true;
  createInfo.HasMinimize = true;
  createInfo.HasMaximize = true;
  createInfo.HasSizingFrame = true;
  createInfo.Title = applicationName;
  IWindow* window = app->CreateApplicationWindow(createInfo);
  window->Show();
  return app;
}
}  // namespace Onyx