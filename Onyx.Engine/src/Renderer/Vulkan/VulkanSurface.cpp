#include "pch.h"

#include "VulkanSurface.h"

#include "Platform/VulkanPlatform.h"

namespace Onyx {
namespace Vulkan {
VulkanSurface::VulkanSurface(VulkanRenderer* backend)
    : _instance(backend->GetInstance()), _surface(VK_NULL_HANDLE) {
  _surface = VulkanPlatform::CreateSurface(backend);
}

VulkanSurface::~VulkanSurface() { vkDestroySurfaceKHR(_instance, _surface, nullptr); }
}  // namespace Vulkan
}  // namespace Onyx
