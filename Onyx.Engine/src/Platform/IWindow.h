#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
namespace Platform {
class IApplication;

//! Struct to contain information about how to construct our window.
struct WindowCreateInfo {
  U32 StartPositionX;    //!< Starting X position for the window.
  U32 StartPositionY;    //!< Starting Y position for the window.
  U32 StartWidth;        //!< Starting width for the window.
  U32 StartHeight;       //!< Starting height for the window.
  bool AllowMaximize;    //!< Whether to include a Maximize button or not.
  bool AllowMinimize;    //!< Whether to include a Minimize button or not.
  const wchar_t* Title;  //!< The window's title.
};

//! Generic Window interface, to be implemented on a per-platform basis.
class IWindow {
 public:
  //! Default constructor that all derived classes must override.
  /*!
    \param application A pointer to the application that owns this window.
    \param createInfo A struct of settings determining how to create the window.
   */
  IWindow(IApplication* application, const WindowCreateInfo& createInfo) {}
  virtual ~IWindow() = default;

  //! Show the window, bringing it to the foreground if possible.
  virtual void Show() = 0;

  //! Hide the window, minimizing it if possible.
  virtual void Hide() = 0;

  //! Request that the window is closed as soon as possible.
  virtual void RequestClose() = 0;

  //! Indicates if this window has received a close request.
  /*!
    \return true if close is requested, false otherwise.
   */
  virtual bool CloseRequested() = 0;

  //! Get a native handle to the underlying platform-specific window.
  /*!
    \return A pointer to the underlying platform-specific handle.
   */
  virtual void* GetHandle() = 0;
};
}  // namespace Platform

//! Factory class for creating and destroying our platform-specific windows.
class ONYX_API Window final {
 public:
  //! Create our platform-specific window. To be implemented in platform sources.
  /*!
    \param application A pointer to the application that owns this window.
    \param createInfo A struct of settings determining how to create the window.
    \return A pointer to the created window.
  */
  static Platform::IWindow* CreateApplicationWindow(Platform::IApplication* application,
                                                    const Platform::WindowCreateInfo& createInfo);

  //! Destroy a created window.
  /*!
    \param window A pointer to a previously created window.
  */
  static void DestroyApplicationWindow(Platform::IWindow* window);
};
}  // namespace Onyx