#include "VulkanSurface.h"

#include "Platform/VulkanPlatform.h"

namespace Onyx {
const bool Onyx::VulkanSurface::Initialize(VkInstance instance,
                                           void* windowHandle) {
  _instance = instance;
  _surface = VulkanPlatform::CreateSurface(instance, windowHandle);
  return true;
}

void VulkanSurface::Shutdown() {
  vkDestroySurfaceKHR(_instance, _surface, nullptr);
  _surface = nullptr;
  _instance = nullptr;
}
}  // namespace Onyx