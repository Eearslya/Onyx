#include <Onyx.h>

class Sandbox : public Onyx::Application {
 public:
  Sandbox() {}
  ~Sandbox() {}
};

Onyx::Application* Onyx::CreateApplication() { return new Sandbox(); }