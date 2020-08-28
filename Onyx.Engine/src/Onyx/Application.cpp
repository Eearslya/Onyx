#include "pch.h"

#include "Application.h"

#include <GLFW/glfw3.h>

#include "Onyx/Events/ApplicationEvent.h"

namespace Onyx {
Application::Application() {
  constexpr WindowProps props{"Onyx", 1600, 900};
  m_Window = CreateScope<Window>(props);
  m_Window->SetCallback([this](const Event& e) { OnEvent(e); });
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

void Application::OnEvent(const Event& e) {
  switch (e.GetEventType()) {
    case EventType::WindowClosed:
      m_Running = false;
      break;
  }
}
}  // namespace Onyx