#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace Onyx {
struct UniformBufferObject {
  glm::mat4 Model;
  glm::mat4 View;
  glm::mat4 Projection;
};
}  // namespace Onyx