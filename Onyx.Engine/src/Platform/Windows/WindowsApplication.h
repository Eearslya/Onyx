#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <Windows.h>

#include "Platform/IApplication.h"

namespace Onyx {
class WindowsWindow;

class WindowsApplication final : public IApplication {
 public:
  WindowsApplication(const wchar_t* applicationName);
  ~WindowsApplication();

  void Run() override;

  void* GetHandle() override { return static_cast<void*>(_hInstance); }

 private:
  HINSTANCE _hInstance;
  WindowsWindow* _mainWindow;
};
}  // namespace Onyx