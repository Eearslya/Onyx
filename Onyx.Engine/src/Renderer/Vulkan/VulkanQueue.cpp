#include "pch.h"

#include "VulkanQueue.h"

#include "Renderer/Vulkan/VulkanDevice.h"

namespace Onyx {
namespace Vulkan {
VulkanQueue::VulkanQueue(VulkanDevice* device, U32 index)
    : _device(device), _index(index), _queue(VK_NULL_HANDLE) {
  vkGetDeviceQueue(_device->GetLogicalDevice(), _index, 0, &_queue);
}
}  // namespace Vulkan
}  // namespace Onyx
