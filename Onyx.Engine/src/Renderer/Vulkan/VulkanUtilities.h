#pragma once

#include "Core/Types.h"

#define VK_CHECK(vkcall) \
  { ASSERT(vkcall == VK_SUCCESS); }

namespace Onyx {
//! Convert an Extent2D object into a VkExtent2D for convenience.
FORCEINLINE Extent2D::operator VkExtent2D() { return {Width, Height}; }
}  // namespace Onyx