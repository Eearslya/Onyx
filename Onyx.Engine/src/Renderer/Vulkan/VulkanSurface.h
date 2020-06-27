#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanRenderer;

//! Represents a Vulkan surface upon which the renderer can draw.
class VulkanSurface final {
 public:
  //! Construct a new surface for the given instance.
  /*!
    \param backend The Vulkan renderer that owns this surface.
    \param windowHandle Pointer to a native platform-specific window handle.
  */
  VulkanSurface(VulkanRenderer* backend);
  ~VulkanSurface();

  //! Get our Vulkan surface handle.
  /*!
    \return The Vulkan surface handle.
  */
  VkSurfaceKHR GetSurface() { return _surface; }

 private:
  VkInstance _instance;   //!< Our Vulkan instance.
  VkSurfaceKHR _surface;  //!< Our Vulkan surface.
};
}  // namespace Vulkan
}  // namespace Onyx