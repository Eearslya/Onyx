#pragma once

#include "Onyx/Core.h"
#include "Onyx/Events/Event.h"
#include "Onyx/Layer.h"
#include "Onyx/Window.h"

#include <vector>
#include <memory>

namespace Onyx {
class Application {
 public:
  ONYX_API Application();
  ONYX_API virtual ~Application();

  ONYX_API void Run();
  ONYX_API void OnEvent(const Event& e);

 protected:
  ONYX_API void PushLayer(Ref<Layer> layer);
  ONYX_API void PushOverlay(Ref<Layer> overlay);
  ONYX_API void PopOverlay(Ref<Layer> overlay);
  ONYX_API void PopLayer(Ref<Layer> layer);

 private:
  Scope<Window> m_Window;
  bool m_Running = false;
  std::vector<Ref<Layer>> m_Layers;
  unsigned int m_LayerInsertIndex = 0;
};

Application* CreateApplication();
}  // namespace Onyx