#pragma once

#include <functional>

#include "Onyx/Core.h"
#include "Onyx/Events/Event.h"

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
  void SetCallback(std::function<void(const Event&)> e);

  unsigned int GetWidth() const;
  unsigned int GetHeight() const;

  void SetVSync(bool enabled);
  bool IsVSync() const;
  bool CloseRequested() const;

  void* GetNativeHandle() const;

 private:
  WindowData* m_Data;
};
}  // namespace Onyx