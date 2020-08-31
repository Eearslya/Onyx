#pragma once

#include "Onyx/Core.h"
#include "Onyx/Layer.h"

namespace Onyx {
class MouseMovedEvent;
class MousePressedEvent;
class MouseReleasedEvent;
class MouseScrolledEvent;
class KeyPressedEvent;
class KeyReleasedEvent;
class KeyTypedEvent;
class WindowResizedEvent;

class ONYX_API ImGuiLayer final : public Layer {
 public:
  ImGuiLayer() = default;
  ~ImGuiLayer() = default;

  void OnAttach() override;
  void OnDetach() override;
  void OnImGuiRender() override;

  void Begin();
  void End();
};
}  // namespace Onyx