#include "pch.h"

#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "Onyx/Application.h"
#include "Onyx/Events/ApplicationEvent.h"
#include "Onyx/Events/KeyEvent.h"
#include "Onyx/Events/MouseEvent.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

#ifdef ONYX_PLATFORM_WINDOWS
#include "Platform/Windows/ImGuiWindowsRenderer.h"
#endif

namespace Onyx {
void Onyx::ImGuiLayer::OnAttach() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();

  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  Application& app = Application::Get();

#ifdef ONYX_PLATFORM_WINDOWS
  GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow()->GetNativeHandle());
  ImGui_ImplGlfw_InitForOpenGL(window, true);
#endif

  // TODO: Renderer platform choosing
  ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach() {
  ImGui_ImplOpenGL3_Shutdown();
#ifdef ONYX_PLATFORM_WINDOWS
  ImGui_ImplGlfw_Shutdown();
#endif
  ImGui::DestroyContext();
}

void ImGuiLayer::OnImGuiRender() {
  static bool show = true;
  ImGui::ShowDemoWindow(&show);
}

void ImGuiLayer::Begin() {
  // TODO: Renderer platform choosing
  ImGui_ImplOpenGL3_NewFrame();
#ifdef ONYX_PLATFORM_WINDOWS
  ImGui_ImplGlfw_NewFrame();
#endif
  ImGui::NewFrame();
}

void ImGuiLayer::End() {
  ImGuiIO& io = ImGui::GetIO();
  Application& app = Application::Get();
  Scope<Window>& win = app.GetWindow();
  auto winWidth = static_cast<float>(win->GetWidth());
  auto winHeight = static_cast<float>(win->GetHeight());
  if (winWidth <= 0.0f || winHeight <= 0.0f) {
    return;
  }
  io.DisplaySize = ImVec2(winWidth, winHeight);

  ImGui::Render();
  // TODO: Renderer platform choosing
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
#ifdef ONYX_PLATFORM_WINDOWS
    GLFWwindow* backupContext = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backupContext);
#endif
  }
}
}  // namespace Onyx