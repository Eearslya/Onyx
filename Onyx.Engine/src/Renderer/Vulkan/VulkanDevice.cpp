#include "pch.h"

#include "VulkanDevice.h"

#include <set>

#include "Renderer/Vulkan/VulkanPhysicalDevice.h"
#include "Renderer/Vulkan/VulkanQueue.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
const std::vector<const char*> VulkanDevice::_requiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VulkanDevice::VulkanDevice(VkInstance instance, const bool validationEnabled,
                           const std::vector<const char*> requiredLayers, VulkanSurface* surface)
    : _instance(instance), _validationEnabled(validationEnabled), _surface(surface) {
  if (!VulkanPhysicalDevice::SelectPhysicalDevice(_instance, _surface, _requiredExtensions,
                                                  _physicalDeviceDetails)) {
    Logger::Fatal("Failed to initialize logical device: No physical device selected.");
  }
  _physicalDevice = _physicalDeviceDetails.Device;
  Logger::Info("Selected device: %s", _physicalDeviceDetails.Properties.deviceName);

  if (!CreateLogicalDevice(requiredLayers)) {
    Logger::Fatal("Failed to create logical device!");
  }

  GetQueues();
}

VulkanDevice::~VulkanDevice() {
  DestroyQueues();
  DestroyLogicalDevice();
}

const bool VulkanDevice::CreateLogicalDevice(const std::vector<const char*> requiredLayers) {
  // Use a set to automatically remove any duplicates.
  std::set<U32> indices;
  indices.insert(_physicalDeviceDetails.Queues.GraphicsQueue);
  indices.insert(_physicalDeviceDetails.Queues.PresentQueue);
  indices.insert(_physicalDeviceDetails.Queues.TransferQueue);
  if (_physicalDeviceDetails.Queues.ComputeQueue != -1) {
    indices.insert(_physicalDeviceDetails.Queues.ComputeQueue);
  }

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(indices.size());
  U32 i = 0;
  F32 queuePriority = 1.0f;
  for (U32 queueIndex : indices) {
    queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[i].queueFamilyIndex = queueIndex;
    queueCreateInfos[i].queueCount = 1;  // TODO: Look at how multiple queues work?
    queueCreateInfos[i].pQueuePriorities = &queuePriority;
    i++;
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(indices.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(_requiredExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = _requiredExtensions.data();

  if (_validationEnabled) {
    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = requiredLayers.data();
  }

  VK_CHECK(vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device));

  return true;
}

void VulkanDevice::GetQueues() {
  _graphicsQueue = new VulkanQueue(this, _physicalDeviceDetails.Queues.GraphicsQueue);
  _presentQueue = new VulkanQueue(this, _physicalDeviceDetails.Queues.PresentQueue);
  _transferQueue = new VulkanQueue(this, _physicalDeviceDetails.Queues.TransferQueue);
  if (_physicalDeviceDetails.Queues.ComputeQueue != -1) {
    _computeQueue = new VulkanQueue(this, _physicalDeviceDetails.Queues.ComputeQueue);
  }
}

void VulkanDevice::DestroyQueues() {
  delete _computeQueue;
  delete _transferQueue;
  delete _presentQueue;
  delete _graphicsQueue;
}

void VulkanDevice::DestroyLogicalDevice() { vkDestroyDevice(_device, nullptr); }
}  // namespace Vulkan
}  // namespace Onyx
