#include "pch.h"

#include "WindowsWindow.h"

#include <GLFW/glfw3.h>

namespace Onyx {
WindowsWindow::WindowsWindow(const WindowProps& props) {
  m_Width = props.Width;
  m_Height = props.Height;

  OnyxInfo("Creating window of size {0}x{1}", m_Width, m_Height);

  OnyxAssert(glfwInit());

  m_Window = glfwCreateWindow(m_Width, m_Height, props.Title, nullptr, nullptr);

  // Center window on the screen
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  int monitorX, monitorY;
  glfwGetMonitorPos(monitor, &monitorX, &monitorY);
  int windowW, windowH;
  glfwGetWindowSize(m_Window, &windowW, &windowH);
  glfwSetWindowPos(m_Window, monitorX + (mode->width - windowW) / 2,
                   monitorY + (mode->height - windowH) / 2);

  glfwMakeContextCurrent(m_Window);
  SetVSync(true);
}

WindowsWindow::~WindowsWindow() { glfwDestroyWindow(m_Window); }

void WindowsWindow::OnUpdate() {
  glfwPollEvents();
  glfwSwapBuffers(m_Window);
}

void WindowsWindow::SetVSync(bool enabled) {
  glfwSwapInterval(enabled ? 1 : 0);
  m_VSync = enabled;
}

bool WindowsWindow::CloseRequested() const { return glfwWindowShouldClose(m_Window); }
}  // namespace Onyx