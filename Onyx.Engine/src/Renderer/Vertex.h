#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace Onyx {
struct Vertex {
  glm::vec2 Position;
  glm::vec3 Color;
};
}  // namespace Onyx