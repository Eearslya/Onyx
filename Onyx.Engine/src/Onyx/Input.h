#pragma once

#include "Onyx/Core.h"
#include "Onyx/KeyCodes.h"
#include "Onyx/MouseButtonCodes.h"

namespace Onyx {
class ONYX_API Input final {
 public:
  static bool IsKeyPressed(Key keycode);
  static bool IsMousePressed(MouseButton button);
  static void GetMousePos(float* x, float* y);
  static float GetMouseX();
  static float GetMouseY();
};
}  // namespace Onyx