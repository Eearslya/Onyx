#pragma once

#include "Onyx/Core.h"
#include "Onyx/Events/Event.h"

namespace Onyx {
class ONYX_API Layer {
 public:
  Layer() = default;
  virtual ~Layer() = default;

  void OnAttach() {}
  void OnDetach() {}
  void OnUpdate() {}
  bool OnEvent(const Event& e) { return false; }
};
}  // namespace Onyx