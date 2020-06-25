#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;
class VulkanSwapchain;

//! Represents a single render pass.
class VulkanRenderPass final {
 public:
  //! Construct a new VulkanRenderPass.
  /*!
    \param device The Vulkan device which owns this render pass.
    \param swapchain The swapchain which will be compatible with this render pass.
  */
  VulkanRenderPass(VulkanDevice* device, VulkanSwapchain* swapchain);
  ~VulkanRenderPass();

  //! Get the extent of our render pass, in pixels.
  VkExtent2D GetExtent();

  //! Get our Vulkan render pass handle.
  VkRenderPass GetRenderPass() { return _renderPass; }

 private:
  VulkanDevice* _device;        //!< Our parent device.
  VulkanSwapchain* _swapchain;  //!< Our compatible swapchain.
  VkRenderPass _renderPass;     //!< Our render pass handle.
};
}  // namespace Vulkan
}  // namespace Onyx