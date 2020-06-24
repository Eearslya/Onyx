#include "pch.h"

#include "VulkanSurface.h"

#include "Platform/VulkanPlatform.h"

namespace Onyx {
namespace Vulkan {
VulkanSurface::VulkanSurface(VulkanRendererBackend* backend, void* windowHandle)
    : _instance(backend->GetInstance()), _surface(VK_NULL_HANDLE) {
  _surface = VulkanPlatform::CreateSurface(backend, windowHandle);
}

VulkanSurface::~VulkanSurface() { vkDestroySurfaceKHR(_instance, _surface, nullptr); }
}  // namespace Vulkan
}  // namespace Onyx
