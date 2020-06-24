#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
class IApplication;

struct WindowCreateInfo {
  U32 StartPositionX;
  U32 StartPositionY;
  U32 StartWidth;
  U32 StartHeight;
  bool AllowMaximize;
  bool AllowMinimize;
  const wchar_t* Title;
};

class IWindow {
 public:
  IWindow(IApplication* application, const WindowCreateInfo& createInfo) {}

  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void RequestClose() = 0;

  virtual bool CloseRequested() = 0;
  virtual void* GetHandle() = 0;
};

class ONYX_API Window final {
 public:
  static IWindow* CreateApplicationWindow(IApplication* application,
                                          const WindowCreateInfo& createInfo);
  static void DestroyApplicationWindow(IWindow* window);
};
}  // namespace Onyx