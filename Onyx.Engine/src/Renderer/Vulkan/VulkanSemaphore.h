#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;

//! Represents a Vulkan semaphore.
class VulkanSemaphore final {
 public:
  //! Construct a new VulkanSemaphore.
  /*!
    \param device The Vulkan device that owns this sempahore.
  */
  VulkanSemaphore(VulkanDevice* device);
  ~VulkanSemaphore();

  //! Get a handle to our semaphore.
  VkSemaphore GetSemaphore() { return _semaphore; }

 private:
  VulkanDevice* _device;   //!< Our parent device.
  VkSemaphore _semaphore;  //!< Our semaphore handle.
};
}  // namespace Vulkan
}  // namespace Onyx