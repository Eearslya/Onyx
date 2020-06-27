#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;

class VulkanBuffer {
 public:
  virtual ~VulkanBuffer() = default;

  const bool Create(VulkanDevice& device, size_t size);
  void Destroy(VulkanDevice& device);

  void Upload(VulkanDevice& device, void* data, size_t size, size_t offset = 0);
  void Map(VulkanDevice& device, void** data, size_t offset = 0);
  void Unmap(VulkanDevice& device);

  virtual VkBufferUsageFlags GetBufferUsage() { return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; }

  operator VkBuffer() { return m_Buffer; }

 private:
  VkBuffer m_Buffer;
  VkDeviceMemory m_DeviceMemory;
  size_t m_Size;
};

class VulkanVertexBuffer : public VulkanBuffer {
 public:
  VkBufferUsageFlags GetBufferUsage() override { return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; }
};

class VulkanIndexBuffer : public VulkanBuffer {
 public:
  VkBufferUsageFlags GetBufferUsage() override { return VK_BUFFER_USAGE_INDEX_BUFFER_BIT; }
};
}  // namespace Vulkan
}  // namespace Onyx