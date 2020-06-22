#pragma once

#include "Defines.h"
#include "Types.h"

namespace Onyx {
struct WindowCreateInfo;
class IWindow;

class ONYX_API IApplication {
 public:
  virtual const bool Initialize() = 0;
  virtual void Run() = 0;
  virtual void PumpMessages(const F32 deltaTime) = 0;
  virtual IWindow* CreateApplicationWindow(
      const WindowCreateInfo& createInfo) = 0;
  virtual IWindow* GetApplicationWindow() = 0;
};

class ONYX_API Application final {
 public:
  static IApplication* CreateApplication(const wchar_t* applicationName);

 private:
  Application() {}
  ~Application() {}
};
}  // namespace Onyx