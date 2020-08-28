#include <Onyx.h>

class SandboxLayer : public Onyx::Layer {};

class Sandbox : public Onyx::Application {
 public:
  Sandbox() { PushLayer(Onyx::CreateRef<SandboxLayer>()); }
  ~Sandbox() {}
};

Onyx::Application* Onyx::CreateApplication() { return new Sandbox(); }