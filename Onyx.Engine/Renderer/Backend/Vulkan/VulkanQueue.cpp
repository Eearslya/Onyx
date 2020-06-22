#include "VulkanQueue.h"

#include "Renderer/Backend/Vulkan/VulkanDevice.h"

const bool Onyx::VulkanQueue::Initialize(VulkanDevice* device, U32 index) {
  _device = device;
  _index = index;
  vkGetDeviceQueue(_device->GetLogicalDevice(), index, 0, &_queue);

  return true;
}

void Onyx::VulkanQueue::Shutdown() {
  _device = nullptr;
  _index = 0;
}
