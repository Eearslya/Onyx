#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
namespace Platform {
//! Generic Application interface, to be implemented on a per-platform basis.
class ONYX_API IApplication {
 public:
  virtual ~IApplication() = default;

  //! Initialize our application interface.
  /*!
    \param applicationName The name of the application.
  */
  IApplication(const wchar_t* applicationName) : _applicationName(applicationName) {}

  //! Begin running the application.
  virtual void Run() = 0;

  //! Return the platform-specific native handle for the application.
  /*!
    \return A pointer to the platform-specific handle for the application.
  */
  virtual void* GetHandle() = 0;

 protected:
  //! The name of our application.
  const wchar_t* _applicationName;
};
}  // namespace Platform

//! Factory class to create our platform-specific Application.
class ONYX_API Application final {
 public:
  //! Create our platform-specific Application.
  /*!
    \param applicationName The name of the application.
    \return A pointer to the created application.
  */
  static Platform::IApplication* CreateApplication(const wchar_t* applicationName);
  
  //! Destroy a created Application.
  /*!
    \param app A previously-created Application.
  */
  static void DestroyApplication(Platform::IApplication* app);
};
}  // namespace Onyx