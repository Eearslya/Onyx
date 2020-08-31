#include "pch.h"

#include "Application.h"

#include <glad/glad.h>

#include "Onyx/Events/ApplicationEvent.h"

namespace Onyx {
Application* Application::s_Application = nullptr;

Application::Application() {
  OnyxAssert(s_Application == nullptr, "Application initialized more than once!");
  s_Application = this;

  constexpr WindowProps props{"Onyx", 1600, 900};
  m_Window = CreateScope<Window>(props);
  m_Window->SetCallback([this](const Event& e) { OnEvent(e); });

  m_ImGuiLayer = CreateRef<ImGuiLayer>();
  PushOverlay(m_ImGuiLayer);

  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  unsigned int vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

  unsigned int ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  unsigned int indices[] = {0, 1, 2};
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

Application::~Application() {}

void Application::Run() {
  m_Running = true;

  glClearColor(0.1f, 0.1f, 0.1f, 1);
  while (m_Running) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

    for (auto layer : m_Layers) {
      layer->OnUpdate();
    }

    m_ImGuiLayer->Begin();
    for (auto layer : m_Layers) {
      layer->OnImGuiRender();
    }
    m_ImGuiLayer->End();

    m_Window->OnUpdate();
  }
}

void Application::OnEvent(const Event& e) {
  switch (e.GetEventType()) {
    case EventType::WindowClosed:
      m_Running = false;
      return;
    case EventType::WindowResized:
      auto evt = reinterpret_cast<const WindowResizedEvent&>(e);
      glViewport(0, 0, evt.Width, evt.Height);
      break;
  }

  for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it) {
    if ((*it)->OnEvent(e)) {
      break;
    }
  }
}

void Application::PushLayer(Ref<Layer> layer) {
  m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
  m_LayerInsertIndex++;
  layer->OnAttach();
}

void Application::PushOverlay(Ref<Layer> overlay) {
  m_Layers.emplace_back(overlay);
  overlay->OnAttach();
}

void Application::PopOverlay(Ref<Layer> overlay) {
  auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
  if (it != m_Layers.end()) {
    overlay->OnDetach();
    m_Layers.erase(it);
  }
}

void Application::PopLayer(Ref<Layer> layer) {
  auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
  if (it != m_Layers.end()) {
    layer->OnDetach();
    m_Layers.erase(it);
    m_LayerInsertIndex--;
  }
}
}  // namespace Onyx