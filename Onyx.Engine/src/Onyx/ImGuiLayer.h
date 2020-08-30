#pragma once

#include "Onyx/Core.h"
#include "Onyx/Layer.h"

namespace Onyx {
class ONYX_API ImGuiLayer final : public Layer {
 public:
  ImGuiLayer() = default;
  ~ImGuiLayer() = default;

  void OnAttach() override;
  void OnUpdate() override;

 private:
  float m_Time = 0.0f;
};
}  // namespace Onyx