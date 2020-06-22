#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
class VulkanPlatform {
 public:
  static void CreateSurface(void* windowHandle, VkInstance instance,
                            VkSurfaceKHR* outSurface);
  static void GetRequiredExtensions(
      std::vector<const char*>& requiredExtensions);
};
}  // namespace Onyx