#pragma once

#include <memory>
#include <vector>

#include "Onyx/Core.h"
#include "Onyx/Events/Event.h"
#include "Onyx/ImGuiLayer.h"
#include "Onyx/Layer.h"
#include "Onyx/Window.h"

namespace Onyx {
class Application {
 public:
  ONYX_API Application();
  ONYX_API virtual ~Application();

  ONYX_API void Run();
  ONYX_API void OnEvent(const Event& e);
  ONYX_API Scope<Window>& GetWindow() { return m_Window; }

  static ONYX_API Application& Get() { return *s_Application; }

 protected:
  ONYX_API void PushLayer(Ref<Layer> layer);
  ONYX_API void PushOverlay(Ref<Layer> overlay);
  ONYX_API void PopOverlay(Ref<Layer> overlay);
  ONYX_API void PopLayer(Ref<Layer> layer);

 private:
  Scope<Window> m_Window;
  bool m_Running = false;
  std::vector<Ref<Layer>> m_Layers;
  Ref<ImGuiLayer> m_ImGuiLayer;
  unsigned int m_LayerInsertIndex = 0;

  static Application* s_Application;
};

Application* CreateApplication();
}  // namespace Onyx