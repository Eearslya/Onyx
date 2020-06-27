#include "pch.h"

#include "VulkanCommandBuffer.h"

#include "Renderer/Vulkan/VulkanCommandPool.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanPipeline.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* pool)
    : _device(device),
      _pool(pool),
      _allocated(false),
      _primary(false),
      _commandBuffer(VK_NULL_HANDLE) {}

void VulkanCommandBuffer::Begin() {
  VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  VK_CHECK(vkBeginCommandBuffer(_commandBuffer, &beginInfo));
}

void VulkanCommandBuffer::End() { VK_CHECK(vkEndCommandBuffer(_commandBuffer)); }

void VulkanCommandBuffer::Reset() { vkResetCommandBuffer(_commandBuffer, 0); }

void VulkanCommandBuffer::BeginRenderPass(VulkanRenderPass* renderPass, VkFramebuffer framebuffer) {
  VkClearValue clearColor = {0.0f, 0.0f, 1.0f, 1.0f};
  VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
  renderPassBeginInfo.framebuffer = framebuffer;
  renderPassBeginInfo.renderArea.offset = {0, 0};
  renderPassBeginInfo.renderArea.extent = renderPass->GetExtent();
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::BindPipeline(VulkanPipeline* pipeline) {
  vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
}

void VulkanCommandBuffer::BindVertexBuffers(U32 bufferCount, VulkanVertexBuffer* vertexBuffers,
                                            VkDeviceSize* offsets) {
  std::vector<VkBuffer> buffers(bufferCount);
  for (U32 i = 0; i < bufferCount; i++) {
    buffers[i] = vertexBuffers[i];
  }

  vkCmdBindVertexBuffers(_commandBuffer, 0, bufferCount, buffers.data(), offsets);
}

void VulkanCommandBuffer::BindIndexBuffer(VulkanIndexBuffer& indexBuffer) {
  vkCmdBindIndexBuffer(_commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
}

void VulkanCommandBuffer::Draw(U32 vertexCount, U32 instanceCount, U32 firstVertex,
                               U32 firstInstance) {
  vkCmdDraw(_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::DrawIndexed(U32 indexCount, U32 instanceCount, U32 firstIndex,
                                      U32 indexOffset, U32 instanceOffset) {
  vkCmdDrawIndexed(_commandBuffer, indexCount, instanceCount, firstIndex, indexOffset,
                   instanceOffset);
}

void VulkanCommandBuffer::EndRenderPass() { vkCmdEndRenderPass(_commandBuffer); }

void VulkanCommandBuffer::Allocate(const bool primary) {
  VkCommandBufferAllocateInfo allocInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  allocInfo.commandPool = _pool->GetCommandPool();
  allocInfo.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = 1;
  VK_CHECK(vkAllocateCommandBuffers(_device->GetLogicalDevice(), &allocInfo, &_commandBuffer));
  _allocated = true;
  _primary = primary;
}

void VulkanCommandBuffer::Free() {
  vkFreeCommandBuffers(_device->GetLogicalDevice(), _pool->GetCommandPool(), 1, &_commandBuffer);
  _allocated = false;
  _commandBuffer = VK_NULL_HANDLE;
}

}  // namespace Vulkan
}  // namespace Onyx
