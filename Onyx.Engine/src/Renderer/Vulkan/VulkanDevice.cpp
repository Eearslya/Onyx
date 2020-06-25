#include "pch.h"

#include "VulkanDevice.h"

#include "Renderer/Vulkan/VulkanPhysicalDevice.h"

namespace Onyx {
namespace Vulkan {
VulkanDevice::VulkanDevice(VkInstance instance, const bool validationEnabled,
                           const std::vector<const char*> requiredLayers, VulkanSurface* surface)
    : _instance(instance), _validationEnabled(validationEnabled), _surface(surface) {
  VulkanPhysicalDevice::SelectPhysicalDevice(_instance, _surface, &_physicalDeviceDetails);
  _physicalDevice = _physicalDeviceDetails.Device;
  Logger::Info("Selected device: %s", _physicalDeviceDetails.Properties.deviceName);
}

VulkanDevice::~VulkanDevice() {}
}  // namespace Vulkan
}  // namespace Onyx
