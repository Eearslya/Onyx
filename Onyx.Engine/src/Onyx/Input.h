#pragma once

#include "Onyx/Core.h"

namespace Onyx {
class ONYX_API Input final {
 public:
  static bool IsKeyPressed(int keycode);
  static bool IsMousePressed(int button);
  static void GetMousePos(float* x, float* y);
  static float GetMouseX();
  static float GetMouseY();
};
}  // namespace Onyx