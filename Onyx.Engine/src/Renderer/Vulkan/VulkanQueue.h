#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;

//! Represents a Vulkan device's command queue.
class VulkanQueue final {
 public:
  //! Create a new Vulkan command queue.
  /*!
    \param device The VulkanDevice this queue belongs to.
    \param index The index of the queue family.
  */
  VulkanQueue(VulkanDevice* device, U32 index);
  ~VulkanQueue() = default;

  //! Get our VkQueue handle.
  VkQueue GetQueue() { return _queue; }

  //! Get our queue family index.
  const U32 GetIndex() { return _index; }

 private:
  VulkanDevice* _device;  //!< Our parent device.
  U32 _index;             //!< Our queue family index.
  VkQueue _queue;         //!< Our queue handle.
};
}  // namespace Vulkan
}  // namespace Onyx