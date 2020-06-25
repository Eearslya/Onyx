#include "pch.h"

#include "VulkanCommandPool.h"

#include "Renderer/Vulkan/VulkanCommandBuffer.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanCommandPool::VulkanCommandPool(VulkanDevice* device, const U32 queueFamilyIndex,
                                     const bool buffersCanReset, const bool buffersProtected,
                                     const bool transient)
    : _device(device), _queueFamilyIndex(queueFamilyIndex), _commandPool(VK_NULL_HANDLE) {
  VkCommandPoolCreateInfo poolCreateInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  poolCreateInfo.queueFamilyIndex = _queueFamilyIndex;
  poolCreateInfo.flags = 0;
  if (buffersCanReset) {
    poolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  }
  if (buffersProtected) {
    poolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
  }
  if (transient) {
    poolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  }

  VK_CHECK(
      vkCreateCommandPool(_device->GetLogicalDevice(), &poolCreateInfo, nullptr, &_commandPool));
}

VulkanCommandPool::~VulkanCommandPool() {
  vkDestroyCommandPool(_device->GetLogicalDevice(), _commandPool, nullptr);
}

VulkanCommandBuffer* VulkanCommandPool::AllocateCommandBuffer(const bool primary) {
  // TODO: Command buffer recycling
  return new VulkanCommandBuffer(_device, this);
}

void VulkanCommandPool::FreeCommandBuffer(VulkanCommandBuffer* buffer) {
  // TODO: Command buffer recycling
  delete buffer;
}

}  // namespace Vulkan
}  // namespace Onyx
