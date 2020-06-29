#pragma once

#include "Defines.h"

#include <Windows.h>

namespace Onyx {
class ONYX_API Application final {
 public:
  static const bool Initialize();
  static void Run();
  static void RequestClose();

  static HINSTANCE GetInstance() { return s_Instance; }
  static HWND GetWindow() { return s_Window; }

 private:
  static void ProcessEvents();

  static HINSTANCE s_Instance;
  static HWND s_Window;
  static bool s_CloseRequested;
};
}  // namespace Onyx