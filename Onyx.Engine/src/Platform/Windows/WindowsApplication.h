#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <Windows.h>

#include "Platform/IApplication.h"

namespace Onyx {
class Engine;

namespace Platform {
class WindowsWindow;

//! IApplication implementation for Windows.
class WindowsApplication final : public IApplication {
 public:
  //! Construct a new WindowsApplication.
  /*!
    \param applicationName The name of the application.
  */
  WindowsApplication(const wchar_t* applicationName);
  ~WindowsApplication();

  void Run() override;

  void* GetHandle() override { return static_cast<void*>(_hInstance); }

 private:
  HINSTANCE _hInstance;        //!< Our native application instance.
  WindowsWindow* _mainWindow;  //!< Our main Window.
  Engine* _engine;             //!< The game engine attached to our application.
};
}  // namespace Platform
}  // namespace Onyx