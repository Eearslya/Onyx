#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
//! Generic Application interface, to be implemented on a per-platform basis.
class ONYX_API IApplication {
 public:
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

//! Factory class to create our platform-specific Application.
class ONYX_API Application final {
 public:
  //! Create our platform-specific Application.
  /*!
    \param applicationName The name of the application.
    \return A pointer to the created application.
  */
  static IApplication* CreateApplication(const wchar_t* applicationName);
  
  //! Destroy a created Application.
  /*!
    \param app A previously-created Application.
  */
  static void DestroyApplication(IApplication* app);
};
}  // namespace Onyx