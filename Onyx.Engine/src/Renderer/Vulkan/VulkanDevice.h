#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer/Vulkan/VulkanPhysicalDevice.h"

namespace Onyx {
namespace Vulkan {
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

 private:
  bool _validationEnabled;  //!< Indicates whether additional validation is enabled.

  VkInstance _instance;              //!< The parent Vulkan instance.
  VkPhysicalDevice _physicalDevice;  //!< The selected Vulkan physical device.
  VulkanSurface* _surface;           //!< The drawing surface.
  VulkanPhysicalDeviceDetails
      _physicalDeviceDetails;  //!< Details relating to the selected Vulkan physical device.
};
}  // namespace Vulkan
}  // namespace Onyx