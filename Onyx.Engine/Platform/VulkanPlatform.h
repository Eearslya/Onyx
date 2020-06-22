#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
class VulkanPlatform {
 public:
  static VkSurfaceKHR CreateSurface(VkInstance instance, void* windowHandle);
  static void GetRequiredExtensions(
      std::vector<const char*>& requiredExtensions);
};
}  // namespace Onyx