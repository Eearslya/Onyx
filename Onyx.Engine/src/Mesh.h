#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "RendererTypes.h"

namespace Onyx {
struct VulkanContext;

class Mesh final {
 public:
  const bool Create(const std::vector<Vertex>& vertices, const std::vector<U32>& indices);
  void Upload(const VulkanContext& vkContext);
  void Bind(VkCommandBuffer& cmdBuf);
  void Free(const VulkanContext& vkContext);
  const size_t GetIndexCount() const { return m_Indices.size(); }

 private:
  std::vector<Vertex> m_Vertices;
  std::vector<U32> m_Indices;
  VkBuffer m_MeshBuffer;
  VkDeviceMemory m_DeviceMemory;
  VkDeviceSize m_IndicesOffset;
};
}  // namespace Onyx