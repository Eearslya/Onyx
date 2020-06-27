#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer/Vulkan/VulkanRenderer.h"

namespace Onyx {
namespace Vulkan {
//! Interface for dealing with platform-specific Vulkan objects.
class VulkanPlatform {
 public:
  //! Get a drawable surface for the given native window.
  /*!
    To be implemented per platform.

    \param backend The Vulkan renderer backend.
  */
  static VkSurfaceKHR CreateSurface(VulkanRenderer* backend);

  //! Fill the given vector with any Vulkan extensions required for the platform.
  /*!
    To be implemented per platform.

    \param requiredExtensions A vector of strings which will have required extensions appended to.
  */
  static void GetRequiredExtensions(std::vector<const char*>& requiredExtensions);
};
}  // namespace Vulkan
}  // namespace Onyx