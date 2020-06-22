#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
class VulkanCommandBuffer;
class VulkanDevice;

class VulkanCommandPool final {
 public:
  const bool Initialize(VulkanDevice* device, const U32 queueFamilyIndex,
                        const bool buffersCanReset = true,
                        const bool buffersProtected = false,
                        const bool transient = false);
  void Shutdown();

  VulkanCommandBuffer* AllocateCommandBuffer(const bool isPrimary);
  void FreeCommandBuffer(VulkanCommandBuffer* commandBuffer);

  VkCommandPool GetCommandPool() { return _commandPool; }

 private:
  U32 _queueFamilyIndex;
  VulkanDevice* _device;
  VkCommandPool _commandPool;
  std::vector<VulkanCommandBuffer*> _activeBuffers;
  std::vector<VulkanCommandBuffer*> _availableBuffers;
};
}  // namespace Onyx