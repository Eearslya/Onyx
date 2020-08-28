#include "pch.h"

#include "Application.h"

#include <GLFW/glfw3.h>

#include "Onyx/Events/ApplicationEvent.h"

namespace Onyx {
Application::Application() {
  WindowProps props{"Onyx", 1600, 900};

  m_Window = Window::Create(props);

  EventBus::Listen<WindowClosedEvent>([this](const Event& e) -> bool {
    m_Running = false;
    return true;
  });
}

Application::~Application() {}

void Application::Run() {
  m_Running = true;

  glClearColor(1, 0, 1, 1);
  while (m_Running) {
    glClear(GL_COLOR_BUFFER_BIT);

    m_Window->OnUpdate();
  }
}
}  // namespace Onyx