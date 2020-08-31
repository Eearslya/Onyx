#include <Onyx.h>
#include <imgui.h>

class SandboxLayer : public Onyx::Layer {
 public:
  void OnImGuiRender() override {
    ImGui::Begin("Sandbox");
    ImGui::Text("SandboxLayer");
    ImGui::End();
  }
};

class Sandbox : public Onyx::Application {
 public:
  Sandbox() { PushLayer(Onyx::CreateRef<SandboxLayer>()); }
  ~Sandbox() {}
};

Onyx::Application* Onyx::CreateApplication() { return new Sandbox(); }