#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include "Core/Engine.h"

namespace Onyx {
class ONYX_API Application final {
 public:
  static void Initialize(const wchar_t* applicationName);
  static void Shutdown();

  static void Run();
  static void RequestClose();

  static void* GetHandle();
  static void* GetWindowHandle();
  static Extent2D GetWindowExtent();

 private:
  static void CreateApplicationWindow();
  static void ShowApplicationWindow();
  static void HideApplicationWindow();
  static void DestroyApplicationWindow();

  static void ProcessEvents();

  static const wchar_t* s_ApplicationName;
  static bool s_WindowVisible;
  static bool s_CloseRequested;

  static Application s_Application;
};
}  // namespace Onyx