#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer/Vulkan/VulkanPhysicalDevice.h"

namespace Onyx {
namespace Vulkan {
class VulkanQueue;
class VulkanSurface;

//! Represents the Vulkan logical device.
class VulkanDevice final {
 public:
  //! Construct a new Vulkan device.
  /*!
    \param instance The Vulkan instance this device belongs to.
    \param validationEnabled Whether or not to enable extra validation layers.
    \param requiredLayers A list of layers our devices are required to have available.
    \param surface Our drawing surface.
  */
  VulkanDevice(VkInstance instance, const bool validationEnabled,
               const std::vector<const char*> requiredLayers, VulkanSurface* surface);
  ~VulkanDevice();

  //! Get our physical device.
  VkPhysicalDevice GetPhysicalDevice() { return _physicalDevice; }

  //! Get our logical device.
  VkDevice GetLogicalDevice() { return _device; }

 private:
  //! Create our logical Vulkan device.
  const bool CreateLogicalDevice(const std::vector<const char*> requiredLayers);

  //! Fetch our Queue objects.
  void GetQueues();

  //! Destroy and free our Queue objects.
  void DestroyQueues();

  //! Destroy and free our logical Vulkan device.
  void DestroyLogicalDevice();

  bool _validationEnabled;  //!< Indicates whether additional validation is enabled.

  VkInstance _instance;              //!< The parent Vulkan instance.
  VkPhysicalDevice _physicalDevice;  //!< The selected Vulkan physical device.
  VkDevice _device;                  //!< Our logical device.
  VulkanSurface* _surface;           //!< The drawing surface.
  VulkanPhysicalDeviceDetails
      _physicalDeviceDetails;  //!< Details relating to the selected Vulkan physical device.

  VulkanQueue* _graphicsQueue;  //!< Our Graphics queue.
  VulkanQueue* _presentQueue;   //!< Our Presentation queue.
  VulkanQueue* _transferQueue;  //!< Our Transfer queue.
  VulkanQueue* _computeQueue;   //!< Our Compute queue.

  static const std::vector<const char*>
      _requiredExtensions;  //!< A list of required extensions for our Vulkan devices.
};
}  // namespace Vulkan
}  // namespace Onyx