#pragma once

#include <GLFW/glfw3.h>

#include "Onyx/Window.h"

namespace Onyx {
class ONYX_API WindowsWindow : public Window {
 public:
  WindowsWindow(const WindowProps& props);
  virtual ~WindowsWindow();

  void OnUpdate() override;

  unsigned int GetWidth() const override { return m_Width; };
  unsigned int GetHeight() const override { return m_Height; };

  void SetVSync(bool enabled) override;
  bool IsVSync() const override { return m_VSync; }
  bool CloseRequested() const override;

 private:
  unsigned int m_Width = 0;
  unsigned int m_Height = 0;
  bool m_VSync = false;
  GLFWwindow* m_Window = nullptr;
};
}  // namespace Onyx