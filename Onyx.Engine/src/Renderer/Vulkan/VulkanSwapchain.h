#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;
class VulkanSurface;

struct VulkanSwapchainSupport;

//! Represents our Vulkan swapchain.
class VulkanSwapchain final {
 public:
  //! Construct a new VulkanSwapchain.
  /*!
    \param device The VulkanDevice which owns this swapchain.
    \param surface The VulkanSurface which owns this swapchain.
    \param extent The size of the swapchain images.
  */
  VulkanSwapchain(VulkanDevice* device, VulkanSurface* surface, VkExtent2D extent);
  ~VulkanSwapchain();

  //! Get our chosen image format.
  VkSurfaceFormatKHR GetImageFormat() { return _imageFormat; }

  //! Get our extents.
  VkExtent2D GetExtent() { return _extent; }

 private:
  //! Choose an image format for our swapchain, allowing for preferred formats.
  VkSurfaceFormatKHR ChooseImageFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

  //! Choose a presentation mode for our swapchain, allowing for preferred modes.
  VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availableModes);

  //! Retrieve and store references to our swapchain images.
  void GetImages();

  //! Create and store our swapchain image views.
  void CreateImageViews();

  //! Destroy and cleanup our swapchain image views.
  void DestroyImageViews();

  VulkanDevice* _device;                 //!< Our parent device.
  VulkanSurface* _surface;               //!< Our parent surface.
  VkExtent2D _extent;                    //!< Our swapchain image size.
  VkSurfaceFormatKHR _imageFormat;       //!< Our chosen image format.
  VkPresentModeKHR _presentMode;         //!< Our chosen presentation mode.
  VkSwapchainKHR _swapchain;             //!< A handle to our Vulkan swapchain object.
  U32 _imageCount;                       //!< How many images are in our swapchain.
  std::vector<VkImage> _images;          //!< A list of our swapchain images.
  std::vector<VkImageView> _imageViews;  //!< A list of our swapchain image views.
};
}  // namespace Vulkan
}  // namespace Onyx