#include "pch.h"

#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "Onyx/Application.h"
#include "Onyx/Events/ApplicationEvent.h"
#include "Onyx/Events/KeyEvent.h"
#include "Onyx/Events/MouseEvent.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

namespace Onyx {
void Onyx::ImGuiLayer::OnAttach() {
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGuiIO& io = ImGui::GetIO();
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

  // TODO: Renderer platform choosing
  ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnUpdate() {
  ImGuiIO& io = ImGui::GetIO();
  Application& app = Application::Get();
  Scope<Window>& win = app.GetWindow();
  io.DisplaySize =
      ImVec2(static_cast<float>(win->GetWidth()), static_cast<float>(win->GetHeight()));

  float time = static_cast<float>(glfwGetTime());
  io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
  m_Time = time;

  // TODO: Renderer platform choosing
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  static bool show = true;
  ImGui::ShowDemoWindow(&show);

  ImGui::Render();
  // TODO: Renderer platform choosing
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiLayer::OnEvent(const Event& e) {
  switch (e.GetEventType()) {
    case EventType::MousePressed:
      OnMousePressed(reinterpret_cast<const MousePressedEvent&>(e));
      break;
    case EventType::MouseReleased:
      OnMouseReleased(reinterpret_cast<const MouseReleasedEvent&>(e));
      break;
    case EventType::MouseMoved:
      OnMouseMoved(reinterpret_cast<const MouseMovedEvent&>(e));
      break;
    case EventType::MouseScrolled:
      OnMouseScrolled(reinterpret_cast<const MouseScrolledEvent&>(e));
      break;
    case EventType::KeyPressed:
      OnKeyPressed(reinterpret_cast<const KeyPressedEvent&>(e));
      break;
    case EventType::KeyReleased:
      OnKeyReleased(reinterpret_cast<const KeyReleasedEvent&>(e));
      break;
    case EventType::KeyTyped:
      OnKeyTyped(reinterpret_cast<const KeyTypedEvent&>(e));
      break;
    case EventType::WindowResized:
      OnWindowResized(reinterpret_cast<const WindowResizedEvent&>(e));
      break;
  }

  return false;
}

bool ImGuiLayer::OnMousePressed(const MousePressedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MouseDown[e.Button] = true;

  return false;
}

bool ImGuiLayer::OnMouseReleased(const MouseReleasedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MouseDown[e.Button] = false;

  return false;
}

bool ImGuiLayer::OnMouseMoved(const MouseMovedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MousePos = ImVec2(static_cast<float>(e.X), static_cast<float>(e.Y));

  return false;
}

bool ImGuiLayer::OnMouseScrolled(const MouseScrolledEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.MouseWheel += static_cast<float>(e.Offset);

  return false;
}

bool ImGuiLayer::OnKeyPressed(const KeyPressedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[e.KeyCode] = true;

  io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
  io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

  return false;
}

bool ImGuiLayer::OnKeyReleased(const KeyReleasedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.KeysDown[e.KeyCode] = false;

  return false;
}

bool ImGuiLayer::OnKeyTyped(const KeyTypedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  if (e.KeyCode > 0 && e.KeyCode < 0x10000) {
    io.AddInputCharacter(static_cast<unsigned short>(e.KeyCode));
  }

  return false;
}

bool ImGuiLayer::OnWindowResized(const WindowResizedEvent& e) { 
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(static_cast<float>(e.Width), static_cast<float>(e.Height));
  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

  return false; }
}  // namespace Onyx