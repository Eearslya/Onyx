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
  void OnUpdate() override;
  bool OnEvent(const Event& e) override;

 private:
  bool OnMousePressed(const MousePressedEvent& e);
  bool OnMouseReleased(const MouseReleasedEvent& e);
  bool OnMouseMoved(const MouseMovedEvent& e);
  bool OnMouseScrolled(const MouseScrolledEvent& e);
  bool OnKeyPressed(const KeyPressedEvent& e);
  bool OnKeyReleased(const KeyReleasedEvent & e);
  bool OnKeyTyped(const KeyTypedEvent& e);
  bool OnWindowResized(const WindowResizedEvent& e);

  float m_Time = 0.0f;
};
}  // namespace Onyx