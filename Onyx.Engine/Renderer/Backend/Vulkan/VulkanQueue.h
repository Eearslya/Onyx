#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
class VulkanDevice;

class VulkanQueue {
 public:
  const bool Initialize(VulkanDevice* device, U32 index);
  void Shutdown();

  VkQueue GetQueue() { return _queue; }
  const U32 GetIndex() { return _index; }

 private:
  VulkanDevice* _device;
  U32 _index;
  VkQueue _queue;
};
}  // namespace Onyx