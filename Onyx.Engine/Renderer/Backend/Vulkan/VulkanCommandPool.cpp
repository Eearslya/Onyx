#include "VulkanCommandPool.h"

#include "Renderer/Backend/Vulkan/VulkanDevice.h"

namespace Onyx {
const bool VulkanCommandPool::Initialize(VulkanDevice* device,
                                               const U32 queueFamilyIndex,
                                               const bool buffersCanReset,
                                               const bool buffersProtected,
                                               const bool transient) {
  _device = device;
  _queueFamilyIndex = queueFamilyIndex;

  VkCommandPoolCreateInfo poolCreateInfo{
      VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
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

  vkCreateCommandPool(_device->GetLogicalDevice(), &poolCreateInfo, nullptr,
                      &_commandPool);

  return true;
}

void VulkanCommandPool::Shutdown() {
  if (_commandPool) {
    vkDestroyCommandPool(_device->GetLogicalDevice(), _commandPool, nullptr);
    _commandPool = nullptr;
  }
  _device = nullptr;
  _queueFamilyIndex = 0;
}

VulkanCommandBuffer* VulkanCommandPool::AllocateCommandBuffer(
    const bool isPrimary) {
  return nullptr;
}

void VulkanCommandPool::FreeCommandBuffer(VulkanCommandBuffer* commandBuffer) {}
}  // namespace Onyx