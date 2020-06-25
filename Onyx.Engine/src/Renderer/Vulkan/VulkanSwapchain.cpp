#include "pch.h"

#include "VulkanSwapchain.h"

#include <algorithm>

#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanPhysicalDevice.h"
#include "Renderer/Vulkan/VulkanQueue.h"
#include "Renderer/Vulkan/VulkanSurface.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, VulkanSurface* surface, VkExtent2D extent)
    : _device(device), _surface(surface), _extent(extent) {
  const VulkanSwapchainSupport& support = _device->GetPhysicalDeviceDetails().SwapchainSupport;

  _imageFormat = ChooseImageFormat(support.Formats);
  _presentMode = ChoosePresentMode(support.PresentModes);

  U32 imageCount =
      std::min(support.Capabilities.minImageCount + 1, support.Capabilities.maxImageCount);

  if (support.Capabilities.currentExtent.width != U32_MAX) {
    _extent = support.Capabilities.currentExtent;
  } else {
    _extent.width = std::clamp(_extent.width, support.Capabilities.minImageExtent.width,
                               support.Capabilities.maxImageExtent.width);
    _extent.height = std::clamp(_extent.height, support.Capabilities.minImageExtent.height,
                                support.Capabilities.maxImageExtent.height);
  }

  VulkanQueue* graphicsQueue = _device->GetGraphicsQueue();
  VulkanQueue* presentQueue = _device->GetPresentQueue();

  VkSwapchainCreateInfoKHR swapchainCreateInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swapchainCreateInfo.surface = _surface->GetSurface();
  swapchainCreateInfo.minImageCount = imageCount;
  swapchainCreateInfo.imageFormat = _imageFormat.format;
  swapchainCreateInfo.imageColorSpace = _imageFormat.colorSpace;
  swapchainCreateInfo.imageExtent = _extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCreateInfo.preTransform = support.Capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = _presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;  // TODO: Use this?

  U32 indices[] = {graphicsQueue->GetIndex(), presentQueue->GetIndex()};
  if (graphicsQueue->GetIndex() == presentQueue->GetIndex()) {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
  } else {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = indices;
  }

  VK_CHECK(vkCreateSwapchainKHR(_device->GetLogicalDevice(), &swapchainCreateInfo, nullptr,
                                &_swapchain));

  GetImages();
  CreateImageViews();
}

VulkanSwapchain::~VulkanSwapchain() {
  DestroyImageViews();
  vkDestroySwapchainKHR(_device->GetLogicalDevice(), _swapchain, nullptr);
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseImageFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const VkSurfaceFormatKHR& format : availableFormats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(
    const std::vector<VkPresentModeKHR>& availableModes) {
  for (const VkPresentModeKHR& mode : availableModes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

void VulkanSwapchain::GetImages() {
  vkGetSwapchainImagesKHR(_device->GetLogicalDevice(), _swapchain, &_imageCount, nullptr);
  _images.resize(_imageCount);
  vkGetSwapchainImagesKHR(_device->GetLogicalDevice(), _swapchain, &_imageCount, _images.data());
}

void VulkanSwapchain::CreateImageViews() {
  _imageViews.resize(_imageCount);
  for (U32 i = 0; i < _imageCount; i++) {
    VkImageViewCreateInfo createInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    createInfo.image = _images[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = _imageFormat.format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(_device->GetLogicalDevice(), &createInfo, nullptr, &_imageViews[i]));
  }
}

void VulkanSwapchain::DestroyImageViews() {
  for (U32 i = 0; i < _imageCount; i++) {
    vkDestroyImageView(_device->GetLogicalDevice(), _imageViews[i], nullptr);
  }
}
}  // namespace Vulkan
}  // namespace Onyx
