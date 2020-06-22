#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
class VulkanCommandPool;
class VulkanQueue;
class VulkanSurface;

struct VulkanSwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR Capabilities;
  std::vector<VkSurfaceFormatKHR> Formats;
  std::vector<VkPresentModeKHR> PresentationModes;
};

class VulkanDevice final {
 public:
  VulkanDevice();
  ~VulkanDevice();

  const bool Initialize(VkInstance instance, const bool validationEnabled,
                        const std::vector<const char*>& requiredLayers,
                        VulkanSurface* surface);
  void Shutdown();

  void SetFramebufferSize(const Extent2D& framebufferSize) {
    _framebufferSize = framebufferSize;
  }
  Extent2D GetFramebufferSize() { return _framebufferSize; }
  VkDevice GetLogicalDevice() { return _device; }

 private:
  bool SelectPhysicalDevice();
  bool IsPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice,
                                VkPhysicalDeviceProperties* properties,
                                VkPhysicalDeviceFeatures* features);
  void DetectQueueFamilyIndices(VkPhysicalDevice physicalDevice,
                                I32* graphicsQueueIndex,
                                I32* presentationQueueIndex);
  VulkanSwapchainSupportDetails QuerySwapchainSupport(
      VkPhysicalDevice physicalDevice);
  bool CreateLogicalDevice(const std::vector<const char*>& requiredLayers);
  bool CreateQueues();
  bool CreateCommandPool();
  bool DetectDepthFormat();

  bool _validationEnabled;
  I32 _graphicsQueueIndex;
  I32 _presentationQueueIndex;
  VkInstance _instance;
  VkPhysicalDevice _physicalDevice;
  VkDevice _device;
  VkSurfaceKHR _surface;
  VulkanQueue* _graphicsQueue;
  VulkanQueue* _presentationQueue;
  VulkanCommandPool* _commandPool;
  VkFormat _depthFormat;
  Extent2D _framebufferSize;  // Temporary?
};
}  // namespace Onyx