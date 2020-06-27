#include "pch.h"

#include "VulkanUtilities.h"

#include "Renderer/Vulkan/VulkanDevice.h"

namespace Onyx {
namespace Vulkan {
U32 Onyx::Vulkan::FindMemoryType(VulkanDevice& device, U32 typeFilter,
                                 VkMemoryPropertyFlags properties) {
  auto memoryProperties = device.GetMemoryProperties();

  for (U32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  Logger::Fatal("Could not find memory type!");
  ASSERT(false);
  return -1;
}
}  // namespace Vulkan
}  // namespace Onyx