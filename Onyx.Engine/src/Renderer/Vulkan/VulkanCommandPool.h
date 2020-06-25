#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanCommandBuffer;
class VulkanDevice;

//! Represents a Vulkan command pool.
class VulkanCommandPool final {
 public:
  //! Create a new command pool.
  /*!
    \param device The Vulkan device that owns this command pool.
    \param queueFamilyIndex The queue family that this command pool will run on.
    \param buffersCanReset Whether or not to allow child buffers to reset.
    \param buffersProtected Whether or not to mark child buffers as protected.
    \param transient Whether or not to mark child buffers as transient.
  */
  VulkanCommandPool(VulkanDevice* device, const U32 queueFamilyIndex, const bool buffersCanReset,
                    const bool buffersProtected, const bool transient);
  ~VulkanCommandPool();

  //! Allocate and return a new buffer from this pool.
  VulkanCommandBuffer* AllocateCommandBuffer(const bool primary);

  //! Free a buffer allocated from this pool.
  void FreeCommandBuffer(VulkanCommandBuffer* buffer);

  //! Get our command pool handle.
  VkCommandPool GetCommandPool() { return _commandPool; }

 private:
  VulkanDevice* _device;       //!< Our parent device.
  U32 _queueFamilyIndex;       //!< Our queue family.
  VkCommandPool _commandPool;  //!< Our command pool handle.
};
}  // namespace Vulkan
}  // namespace Onyx