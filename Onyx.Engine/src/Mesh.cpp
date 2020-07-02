#include "Mesh.h"

#include "Renderer.h"

namespace Onyx {
const bool Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<U32>& indices) {
  m_Vertices = vertices;
  m_Indices = indices;

  return true;
}

void Mesh::Upload(const VulkanContext& vkContext) {
  VkDeviceSize vertexSize = sizeof(m_Vertices[0]) * m_Vertices.size();
  VkDeviceSize indexSize = sizeof(m_Indices[0]) * m_Indices.size();
  VkDeviceSize bufferSize = vertexSize + indexSize;
  m_IndicesOffset = vertexSize;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingDeviceMemory;
  ASSERT(Renderer::CreateBuffer(
      bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
      stagingDeviceMemory));
  void* data;
  vkMapMemory(vkContext.Device, stagingDeviceMemory, 0, vertexSize, 0, &data);
  memcpy(data, m_Vertices.data(), static_cast<size_t>(vertexSize));
  vkUnmapMemory(vkContext.Device, stagingDeviceMemory);
  vkMapMemory(vkContext.Device, stagingDeviceMemory, vertexSize, indexSize, 0, &data);
  memcpy(data, m_Indices.data(), static_cast<size_t>(indexSize));
  vkUnmapMemory(vkContext.Device, stagingDeviceMemory);

  ASSERT(Renderer::CreateBuffer(bufferSize,
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_MeshBuffer, m_DeviceMemory));
  Renderer::CopyBuffer(stagingBuffer, m_MeshBuffer, bufferSize);

  vkDestroyBuffer(vkContext.Device, stagingBuffer, nullptr);
  vkFreeMemory(vkContext.Device, stagingDeviceMemory, nullptr);
}

void Mesh::Bind(VkCommandBuffer& cmdBuf) {
  const U64 offsets = 0;
  vkCmdBindVertexBuffers(cmdBuf, 0, 1, &m_MeshBuffer, &offsets);
  vkCmdBindIndexBuffer(cmdBuf, m_MeshBuffer, m_IndicesOffset, VK_INDEX_TYPE_UINT32);
}

void Mesh::Free(const VulkanContext& vkContext) {
  vkDestroyBuffer(vkContext.Device, m_MeshBuffer, nullptr);
  vkFreeMemory(vkContext.Device, m_DeviceMemory, nullptr);
}
}  // namespace Onyx