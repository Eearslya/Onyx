#include "pch.h"

#ifdef ONYX_PLATFORM_WINDOWS

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Onyx/Events/ApplicationEvent.h"
#include "Onyx/Events/KeyEvent.h"
#include "Onyx/Events/MouseEvent.h"
#include "Onyx/Window.h"

namespace Onyx {
struct WindowData {
  GLFWwindow* Window = nullptr;
  unsigned int Width = 0;
  unsigned int Height = 0;
  bool VSync = false;
  std::function<void(const Event&)> Callback = nullptr;
};

static void GLFWError(int error, const char* description) {
  OnyxError("GLFW Error {0}: {1}", error, description);
}

Window::Window(const WindowProps& props) {
  m_Data = new WindowData();
  m_Data->Width = props.Width;
  m_Data->Height = props.Height;

  OnyxInfo("Creating window of size {0}x{1}", m_Data->Width, m_Data->Height);

  int init = glfwInit();
  OnyxAssert(init, "Failed to initialize GLFW");
  glfwSetErrorCallback(GLFWError);

  m_Data->Window = glfwCreateWindow(m_Data->Width, m_Data->Height, props.Title, nullptr, nullptr);

  // Center window on the screen
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  int monitorX, monitorY;
  glfwGetMonitorPos(monitor, &monitorX, &monitorY);
  int windowW, windowH;
  glfwGetWindowSize(m_Data->Window, &windowW, &windowH);
  glfwSetWindowPos(m_Data->Window, monitorX + (mode->width - windowW) / 2,
                   monitorY + (mode->height - windowH) / 2);

  glfwMakeContextCurrent(m_Data->Window);
  glfwSetWindowUserPointer(m_Data->Window, m_Data);
  SetVSync(true);
  int glad = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  OnyxAssert(glad, "Failed to initialize Glad");

  glfwSetWindowSizeCallback(m_Data->Window, [](GLFWwindow* window, int width, int height) {
    WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    data.Width = width;
    data.Height = height;

    if (!data.Callback) {
      return;
    }

    data.Callback(WindowResizedEvent(width, height));
  });

  glfwSetWindowCloseCallback(m_Data->Window, [](GLFWwindow* window) {
    WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if (!data.Callback) {
      return;
    }

    data.Callback(WindowClosedEvent());
  });

  glfwSetKeyCallback(
      m_Data->Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (!data.Callback) {
          return;
        }

        switch (action) {
          case GLFW_PRESS:
          case GLFW_REPEAT:
            data.Callback(KeyPressedEvent(key));
            break;
          case GLFW_RELEASE:
            data.Callback(KeyReleasedEvent(key));
            break;
        }
      });

  glfwSetMouseButtonCallback(
      m_Data->Window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (!data.Callback) {
          return;
        }

        switch (action) {
          case GLFW_PRESS:
            data.Callback(MousePressedEvent(button));
            break;
          case GLFW_RELEASE:
            data.Callback(MouseReleasedEvent(button));
            break;
        }
      });

  glfwSetScrollCallback(m_Data->Window, [](GLFWwindow* window, double x, double y) {
    WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if (!data.Callback) {
      return;
    }

    data.Callback(MouseScrolledEvent(y));
  });

  glfwSetCursorPosCallback(m_Data->Window, [](GLFWwindow* window, double x, double y) {
    WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if (!data.Callback) {
      return;
    }

    data.Callback(MouseMovedEvent(x, y));
  });
}

Window::~Window() {
  glfwDestroyWindow(m_Data->Window);
  delete m_Data;
}

void Window::OnUpdate() {
  glfwPollEvents();
  glfwSwapBuffers(m_Data->Window);
}

void Window::SetCallback(std::function<void(const Event&)> func) { m_Data->Callback = func; }

unsigned int Window::GetWidth() const { return m_Data->Width; };

unsigned int Window::GetHeight() const { return m_Data->Height; };

void Window::SetVSync(bool enabled) {
  glfwSwapInterval(enabled ? 1 : 0);
  m_Data->VSync = enabled;
}

bool Window::IsVSync() const { return m_Data->VSync; }

bool Window::CloseRequested() const { return glfwWindowShouldClose(m_Data->Window); }
}  // namespace Onyx

#endif /* ONYX_PLATFORM_WINDOWS */