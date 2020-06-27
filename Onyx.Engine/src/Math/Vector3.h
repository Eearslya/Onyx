#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Onyx {
struct Vector3 {
  F32 X;
  F32 Y;
  F32 Z;

  Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {}
  Vector3(F32 x, F32 y, F32 z) : X(x), Y(y), Z(z) {}
};
}  // namespace Onyx
