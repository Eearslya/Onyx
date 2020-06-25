#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanCommandPool;
class VulkanDevice;
class VulkanPipeline;
class VulkanRenderPass;

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

  //! vkBeginCommandBuffer
  void Begin();

  //! vkEndCommandBuffer
  void End();

  //! vkCmdBeginRenderPass
  void BeginRenderPass(VulkanRenderPass* renderPass, VkFramebuffer framebuffer);

  //! vkCmdBindPipeline
  void BindPipeline(VulkanPipeline* pipeline);

  //! vkCmdDraw
  void Draw(U32 vertexCount, U32 instanceCount, U32 firstVertex, U32 firstInstance);

  //! vkCmdEndRenderPass
  void EndRenderPass();

  //! Allocate our command buffer.
  void Allocate(const bool primary);

  //! Free our command buffer.
  void Free();

 private:
  VulkanDevice* _device;           //!< Our parent device.
  VulkanCommandPool* _pool;        //!< Our parent pool.
  VkCommandBuffer _commandBuffer;  //!< Our command buffer handle.
  bool _allocated;                 //!< Whether or not we are currently allocated.
  bool _primary;                   //!< Whether we are a primary buffer or not.
};
}  // namespace Vulkan
}  // namespace Onyx