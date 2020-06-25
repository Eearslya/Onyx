#include "pch.h"

#include "VulkanFence.h"

#include "Renderer/Vulkan/VulkanDevice.h"

namespace Onyx {
namespace Vulkan {
VulkanFence::VulkanFence(VulkanDevice* device, bool startSignaled)
    : _device(device), _fence(VK_NULL_HANDLE) {
  VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fenceInfo.flags = startSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
  vkCreateFence(_device->GetLogicalDevice(), &fenceInfo, nullptr, &_fence);
}

VulkanFence::~VulkanFence() { vkDestroyFence(_device->GetLogicalDevice(), _fence, nullptr); }

void VulkanFence::Wait() {
  vkWaitForFences(_device->GetLogicalDevice(), 1, &_fence, VK_TRUE, U64_MAX);
}

void VulkanFence::Reset() { vkResetFences(_device->GetLogicalDevice(), 1, &_fence); }
}  // namespace Vulkan
}  // namespace Onyx
