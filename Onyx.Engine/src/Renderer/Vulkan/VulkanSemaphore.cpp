#include "pch.h"

#include "VulkanSemaphore.h"

#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanSemaphore::VulkanSemaphore(VulkanDevice* device) : _device(device) {
  VkSemaphoreCreateInfo semaphoreInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VK_CHECK(vkCreateSemaphore(_device->GetLogicalDevice(), &semaphoreInfo, nullptr, &_semaphore));
}

VulkanSemaphore::~VulkanSemaphore() {
  vkDestroySemaphore(_device->GetLogicalDevice(), _semaphore, nullptr);
}
}  // namespace Vulkan
}  // namespace Onyx
