#pragma once

#include "Onyx/Core.h"

namespace Onyx {
struct WindowProps {
  const char* Title;
  unsigned int Width;
  unsigned int Height;
};

// Struct to be defined by each platform implementation,
// free to store any and all necessary data for that platform.
struct WindowData;

class ONYX_API Window final {
 public:
  Window(const WindowProps& props);
  ~Window();

  void OnUpdate();

  unsigned int GetWidth() const;
  unsigned int GetHeight() const;

  void SetVSync(bool enabled);
  bool IsVSync() const;
  bool CloseRequested() const;

 private:
  WindowData* m_Data;
};
}  // namespace Onyx