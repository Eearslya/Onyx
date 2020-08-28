#pragma once

#include "Onyx/Core.h"

namespace Onyx {
struct WindowProps {
  const char* Title;
  unsigned int Width;
  unsigned int Height;
};

class ONYX_API Window {
 public:
  virtual ~Window() = default;

  virtual void OnUpdate() = 0;

  virtual unsigned int GetWidth() const = 0;
  virtual unsigned int GetHeight() const = 0;

  virtual void SetVSync(bool enabled) = 0;
  virtual bool IsVSync() const = 0;
  virtual bool CloseRequested() const = 0;

  static Scope<Window> Create(const WindowProps& props);
};
}  // namespace Onyx