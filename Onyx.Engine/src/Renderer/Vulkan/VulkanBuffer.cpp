#include "pch.h"

#include "VulkanBuffer.h"

#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
const bool VulkanBuffer::Create(VulkanDevice& device, size_t size) {
  m_Size = size;

  VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
  bufferInfo.size = size;
  bufferInfo.usage = GetBufferUsage();
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer));

  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);

  VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  allocInfo.allocationSize = memoryRequirements.size;
  allocInfo.memoryTypeIndex =
      FindMemoryType(device, memoryRequirements.memoryTypeBits,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &m_DeviceMemory));

  VK_CHECK(vkBindBufferMemory(device, m_Buffer, m_DeviceMemory, 0));

  return true;
}

void VulkanBuffer::Destroy(VulkanDevice& device) {
  vkFreeMemory(device, m_DeviceMemory, nullptr);
  vkDestroyBuffer(device, m_Buffer, nullptr);

  m_Size = 0;
  m_DeviceMemory = VK_NULL_HANDLE;
  m_Buffer = VK_NULL_HANDLE;
}

void VulkanBuffer::Map(VulkanDevice& device, void** data) {
  vkMapMemory(device, m_DeviceMemory, 0, m_Size, 0, data);
}

void VulkanBuffer::Unmap(VulkanDevice& device) { vkUnmapMemory(device, m_DeviceMemory); }
}  // namespace Vulkan
}  // namespace Onyx
