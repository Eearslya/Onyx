#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include "Renderer/Vulkan/VulkanBuffer.h"

namespace Onyx {
namespace Vulkan {
class VulkanCommandPool;
class VulkanDevice;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanSemaphore;

//! Represents a Vulkan command buffer.
class VulkanCommandBuffer final {
 public:
  //! Construct a new VulkanCommandBuffer.
  /*!
    \param device The Vulkan device which owns this command buffer.
    \param pool The command pool which will be used to allocate this buffer.
  */
  VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* pool);
  ~VulkanCommandBuffer() = default;

  void Begin();
  void End();
  void Reset();
  void BeginRenderPass(VulkanRenderPass* renderPass, VkFramebuffer framebuffer);
  void BindPipeline(VulkanPipeline* pipeline);
  void BindVertexBuffers(U32 bufferCount, VulkanVertexBuffer* vertexBuffers, VkDeviceSize* offsets);
  void BindIndexBuffer(VulkanIndexBuffer& indexBuffer);
  void Draw(U32 vertexCount, U32 instanceCount, U32 firstVertex, U32 firstInstance);
  void DrawIndexed(U32 indexCount, U32 instanceCount = 1, U32 firstIndex = 0, U32 indexOffset = 0,
                   U32 instanceOffset = 0);
  void EndRenderPass();

  void Allocate(const bool primary);
  void Free();

  //! Get our command buffer handle.
  VkCommandBuffer GetCommandBuffer() { return _commandBuffer; }

 private:
  VulkanDevice* _device;           //!< Our parent device.
  VulkanCommandPool* _pool;        //!< Our parent pool.
  VkCommandBuffer _commandBuffer;  //!< Our command buffer handle.
  bool _allocated;                 //!< Whether or not we are currently allocated.
  bool _primary;                   //!< Whether we are a primary buffer or not.
};
}  // namespace Vulkan
}  // namespace Onyx