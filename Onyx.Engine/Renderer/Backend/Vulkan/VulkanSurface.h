#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
class VulkanSurface final {
 public:
  const bool Initialize(VkInstance instance, void* windowHandle);
  void Shutdown();

  VkSurfaceKHR GetSurface() { return _surface; }

 private:
  VkInstance _instance;
  VkSurfaceKHR _surface;
};
}  // namespace Onyx