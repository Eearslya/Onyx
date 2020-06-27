#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
struct Vector2 {
  F32 X;
  F32 Y;

  Vector2() : X(0.0f), Y(0.0f) {}
  Vector2(F32 x, F32 y) : X(x), Y(y) {}
};
}  // namespace Onyx