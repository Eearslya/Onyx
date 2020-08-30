#include <Onyx.h>

class SandboxLayer : public Onyx::Layer {};

class Sandbox : public Onyx::Application {
 public:
  Sandbox() {
    PushLayer(Onyx::CreateRef<SandboxLayer>());
    PushOverlay(Onyx::CreateRef<Onyx::ImGuiLayer>());
  }
  ~Sandbox() {}
};

Onyx::Application* Onyx::CreateApplication() { return new Sandbox(); }