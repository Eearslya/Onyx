#pragma once

#include "Onyx/Core.h"
#include "Onyx/Events/Event.h"

namespace Onyx {
class ONYX_API Layer {
 public:
  Layer() = default;
  virtual ~Layer() = default;

  virtual void OnAttach() {}
  virtual void OnDetach() {}
  virtual void OnUpdate() {}
  virtual bool OnEvent(const Event& e) { return false; }
  virtual void OnImGuiRender() {}
};
}  // namespace Onyx