#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;

class VulkanFence final {
 public:
  VulkanFence(VulkanDevice* device, bool startSignaled = false);
  ~VulkanFence();

  void Wait();
  void Reset();

  VkFence GetFence() { return _fence; }

 private:
  VulkanDevice* _device;
  VkFence _fence;
};
}  // namespace Vulkan
}  // namespace Onyx