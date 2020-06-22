#pragma once

#include "Defines.h"
#include "Types.h"

namespace Onyx {
class IApplication;

/**
 * Structure used to define the starting parameters for a window.
 */
struct WindowCreateInfo {
  /**
   * Starting X position for the window.
   */
  F32 StartPositionX;

  /**
   * Starting Y position for the window.
   */
  F32 StartPositionY;

  /**
   * Starting width for the window.
   */
  F32 StartWidth;

  /**
   * Starting height for the window.
   */
  F32 StartHeight;

  /**
   * Whether the window accepts input from mouse/keyboard.
   */
  bool AcceptsInput;

  /**
   * Whether this window is a normal window, or a special case such as tooltips.
   */
  bool IsStandardWindow;
  bool HasOSWindowBorder;
  bool IsTopmostWindow;
  bool HasCloseButton;
  bool HasMinimize;
  bool HasMaximize;
  bool HasSizingFrame;
  const wchar_t* Title;
};

class IWindow {
 public:
  virtual const Extent2D GetFramebufferExtent() const = 0;
  virtual void* GetHandle() = 0;
  virtual void Initialize(IApplication* application,
                          const WindowCreateInfo& createInfo) = 0;
  virtual void RequestClose() = 0;
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual const bool IsCloseRequested() const = 0;
};
}  // namespace Onyx