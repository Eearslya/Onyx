#include "VulkanDevice.h"

#include <vector>

#include "Logger.h"
#include "Renderer/Backend/Vulkan/VulkanCommandPool.h"
#include "Renderer/Backend/Vulkan/VulkanQueue.h"
#include "Renderer/Backend/Vulkan/VulkanSurface.h"

namespace Onyx {
VulkanDevice::VulkanDevice() {
  _graphicsQueue = new VulkanQueue();
  _presentationQueue = new VulkanQueue();
  _commandPool = new VulkanCommandPool();
}

VulkanDevice::~VulkanDevice() {
  delete _commandPool;
  delete _presentationQueue;
  delete _graphicsQueue;
}

const bool VulkanDevice::Initialize(
    VkInstance instance, const bool validationEnabled,
    const std::vector<const char*>& requiredLayers, VulkanSurface* surface) {
  _instance = instance;
  _surface = surface->GetSurface();
  _validationEnabled = validationEnabled;

  if (!SelectPhysicalDevice()) {
    return false;
  }

  if (!CreateLogicalDevice(requiredLayers)) {
    return false;
  }

  if (!CreateQueues()) {
    return false;
  }

  if (!DetectDepthFormat()) {
    return false;
  }

  return true;
}

void VulkanDevice::Shutdown() {
  if (_commandPool) {
    _commandPool->Shutdown();
  }

  if (_graphicsQueue) {
    _graphicsQueue->Shutdown();
  }
  
  if (_presentationQueue) {
    _presentationQueue->Shutdown();
  }

  vkDestroyDevice(_device, nullptr);

  _physicalDevice = nullptr;
  _device = nullptr;
  _surface = nullptr;
  _instance = nullptr;
}

bool VulkanDevice::SelectPhysicalDevice() {
  U32 physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);
  if (physicalDeviceCount == 0) {
    Logger::Fatal("No supported Vulkan devices were found!");
    return false;
  }
  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount,
                             physicalDevices.data());

  Logger::Debug("SelectPhysicalDevice: Found %d devices.", physicalDeviceCount);

  for (U32 i = 0; i < physicalDeviceCount; i++) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memoryProperties);

    if (IsPhysicalDeviceSuitable(physicalDevices[i], &properties, &features)) {
      Logger::Info("Selected Device: %s", properties.deviceName);
      _physicalDevice = physicalDevices[i];

      switch (properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
          Logger::Debug("-> Device Type: Dedicated");
          break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
          Logger::Debug("-> Device Type: Integrated");
          break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
          Logger::Debug("-> Device Type: Virtual");
          break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
          Logger::Debug("-> Device Type: CPU");
          break;
        default:
          Logger::Debug("-> Device Type: Unknown");
          break;
      }

      for (U32 i = 0; i < memoryProperties.memoryHeapCount; i++) {
        F32 memorySizeMiB =
            (((F32)memoryProperties.memoryHeaps[i].size) / 1024.0f / 1024.0f);
        if (memoryProperties.memoryHeaps[i].flags &
            VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
          Logger::Debug("-> Device Local Memory:  %.2f MiB", memorySizeMiB);
        } else {
          Logger::Debug("-> Device Shared Memory: %.2f MiB", memorySizeMiB);
        }
      }

      return true;
    }
  }

  Logger::Fatal("No physical devices found which meet requirements.");
  return false;
}

bool VulkanDevice::IsPhysicalDeviceSuitable(
    VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* properties,
    VkPhysicalDeviceFeatures* features) {
  I32 graphicsQueueIndex = -1;
  I32 presentationQueueIndex = -1;
  DetectQueueFamilyIndices(physicalDevice, &graphicsQueueIndex,
                           &presentationQueueIndex);
  VulkanSwapchainSupportDetails swapchainSupport =
      QuerySwapchainSupport(physicalDevice);

  // Require that our device have a graphics and presentation queue.
  if (graphicsQueueIndex == -1 || presentationQueueIndex == -1) {
    return false;
  }

  U32 extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::vector<const char*> requiredExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  for (U64 i = 0; i < requiredExtensions.size(); i++) {
    bool found = false;
    for (U64 j = 0; j < extensionCount; j++) {
      if (strcmp(requiredExtensions[i], availableExtensions[j].extensionName) ==
          0) {
        found = true;
        break;
      }
    }

    if (!found) {
      // Failed to find a required extension, bail out.
      return false;
    }
  }

  // Require we have at least one valid format and presentation mode.
  if (swapchainSupport.Formats.size() == 0 ||
      swapchainSupport.PresentationModes.size() == 0) {
    return false;
  }

  // Require Sampler Anisotropy.
  if (!features->samplerAnisotropy) {
    return false;
  }

  // If we've gotten this far, we've got a good device!
  return true;
}

void VulkanDevice::DetectQueueFamilyIndices(VkPhysicalDevice physicalDevice,
                                            I32* graphicsQueueIndex,
                                            I32* presentationQueueIndex) {
  U32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilyProperties.data());

  for (U32 i = 0; i < queueFamilyCount; i++) {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      *graphicsQueueIndex = i;
    }

    VkBool32 supportsPresentation = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _surface,
                                         &supportsPresentation);
    if (supportsPresentation) {
      *presentationQueueIndex = i;
    }
  }
}

VulkanSwapchainSupportDetails VulkanDevice::QuerySwapchainSupport(
    VkPhysicalDevice physicalDevice) {
  VulkanSwapchainSupportDetails support{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _surface,
                                            &support.Capabilities);

  U32 formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount,
                                       nullptr);
  if (formatCount != 0) {
    support.Formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount,
                                         support.Formats.data());
  }

  U32 presentationModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface,
                                            &presentationModeCount, nullptr);
  if (presentationModeCount != 0) {
    support.PresentationModes.resize(presentationModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface,
                                              &presentationModeCount,
                                              support.PresentationModes.data());
  }

  return support;
}

bool VulkanDevice::CreateLogicalDevice(
    const std::vector<const char*>& requiredLayers) {
  I32 graphicsQueueIndex = -1;
  I32 presentationQueueIndex = -1;
  DetectQueueFamilyIndices(_physicalDevice, &graphicsQueueIndex,
                           &presentationQueueIndex);
  ASSERT(graphicsQueueIndex != -1 && presentationQueueIndex != -1);

  bool presentationSharesGraphicsQueue =
      graphicsQueueIndex == presentationQueueIndex;

  std::vector<U32> indices;
  indices.push_back(graphicsQueueIndex);
  if (!presentationSharesGraphicsQueue) {
    indices.push_back(presentationQueueIndex);
  }

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(indices.size());
  F32 queuePriority = 1.0f;
  for (U32 i = 0; i < indices.size(); i++) {
    queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[i].queueFamilyIndex = indices[i];
    queueCreateInfos[i].queueCount = 1;
    queueCreateInfos[i].flags = 0;
    queueCreateInfos[i].pQueuePriorities = &queuePriority;
    queueCreateInfos[i].pNext = nullptr;
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  const char* requiredExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(indices.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount = 1;
  deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions;

  if (_validationEnabled) {
    deviceCreateInfo.enabledLayerCount =
        static_cast<uint32_t>(requiredLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = requiredLayers.data();
  }

  vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device);

  _graphicsQueueIndex = graphicsQueueIndex;
  _presentationQueueIndex = presentationQueueIndex;

  return true;
}

bool VulkanDevice::CreateQueues() {
  _graphicsQueue->Initialize(this, _graphicsQueueIndex);
  _presentationQueue->Initialize(this, _presentationQueueIndex);
  return true;
}

bool VulkanDevice::CreateCommandPool() {
  _commandPool->Initialize(this, _graphicsQueueIndex, true, false, false);
  return true;
}

bool VulkanDevice::DetectDepthFormat() {
  const U64 candidateCount = 3;
  VkFormat candidates[candidateCount] = {VK_FORMAT_D32_SFLOAT,
                                         VK_FORMAT_D32_SFLOAT_S8_UINT,
                                         VK_FORMAT_D24_UNORM_S8_UINT};

  U32 flag = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
  for (U64 i = 0; i < candidateCount; i++) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(_physicalDevice, candidates[i],
                                        &properties);

    if (properties.linearTilingFeatures & flag) {
      _depthFormat = candidates[i];
      return true;
    } else if (properties.optimalTilingFeatures & flag) {
      _depthFormat = candidates[i];
      return true;
    }
  }

  return false;
}
}  // namespace Onyx