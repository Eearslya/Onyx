#include "pch.h"

#include "WindowsWindow.h"

#include <GLFW/glfw3.h>

#include "Onyx/Events/ApplicationEvent.h"
#include "Onyx/Events/KeyEvent.h"
#include "Onyx/Events/MouseEvent.h"

namespace Onyx {
static void GLFWError(int error, const char* description) {
  OnyxError("GLFW Error {0}: {1}", error, description);
}

WindowsWindow::WindowsWindow(const WindowProps& props) {
  m_Width = props.Width;
  m_Height = props.Height;

  OnyxInfo("Creating window of size {0}x{1}", m_Width, m_Height);

  OnyxAssert(glfwInit());
  glfwSetErrorCallback(GLFWError);

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

  glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
    EventBus::Dispatch(WindowResizedEvent(width, height));
  });

  glfwSetWindowCloseCallback(m_Window,
                             [](GLFWwindow* window) { EventBus::Dispatch(WindowClosedEvent()); });

  glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action) {
      case GLFW_PRESS:
      case GLFW_REPEAT:
        EventBus::Dispatch(KeyPressedEvent(key));
        break;
      case GLFW_RELEASE:
        EventBus::Dispatch(KeyReleasedEvent(key));
        break;
    }
  });

  glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
    switch (action) {
      case GLFW_PRESS:
        EventBus::Dispatch(MousePressedEvent(button));
        break;
      case GLFW_RELEASE:
        EventBus::Dispatch(MouseReleasedEvent(button));
        break;
    }
  });

  glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double x, double y) {
    EventBus::Dispatch(MouseScrolledEvent(y));
  });

  glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y) {
    EventBus::Dispatch(MouseMovedEvent(x, y));
  });
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