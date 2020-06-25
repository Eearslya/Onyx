#include "pch.h"

#include "VulkanPhysicalDevice.h"

#include <algorithm>
#include <vector>

#include "Renderer/Vulkan/VulkanSurface.h"

namespace Onyx {
namespace Vulkan {
const std::vector<std::string> VulkanPhysicalDevice::_requiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const bool VulkanPhysicalDevice::SelectPhysicalDevice(VkInstance instance, VulkanSurface* surface,
                                                      VulkanPhysicalDeviceDetails* details) {
  U32 physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
  if (physicalDeviceCount == 0) {
    Logger::Fatal("No Vulkan devices were found!");
    return false;
  }
  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

  Logger::Debug("Found %d Vulkan device(s).", physicalDeviceCount);

  std::vector<VulkanPhysicalDeviceDetails> physicalDeviceDetails(physicalDeviceCount);
  std::vector<VulkanPhysicalDeviceDetails*> candidateDevices;
  for (U32 i = 0; i < physicalDeviceCount; i++) {
    QueryDeviceDetails(physicalDevices[i], surface, physicalDeviceDetails[i]);
#ifdef ONYX_TRACE
    DumpDeviceDetails(physicalDeviceDetails[i]);
#endif
    if (VerifyDeviceCapabilities(physicalDeviceDetails[i])) {
      candidateDevices.push_back(&physicalDeviceDetails[i]);
    }
  }

  if (candidateDevices.size() == 0) {
    Logger::Fatal("No Vulkan devices meet application requirements!");
    return false;
  }

  memcpy(details, candidateDevices[0], sizeof(VulkanPhysicalDeviceDetails));

  return true;
}

void VulkanPhysicalDevice::QueryDeviceDetails(VkPhysicalDevice physicalDevice,
                                              VulkanSurface* surface, 
                                              VulkanPhysicalDeviceDetails& details) {
  vkGetPhysicalDeviceProperties(physicalDevice, &details.Properties);
  vkGetPhysicalDeviceFeatures(physicalDevice, &details.Features);
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &details.MemoryProperties);
  QueryDeviceQueues(physicalDevice, surface, details.Queues);
  QueryDeviceSwapchainSupport(physicalDevice, surface, details.SwapchainSupport);
  QueryDeviceExtensions(physicalDevice, details.Extensions);
}

void VulkanPhysicalDevice::QueryDeviceQueues(VkPhysicalDevice physicalDevice,
                                             VulkanSurface* surface,
                                             VulkanPhysicalDeviceQueues& queues) {
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queues.Count, nullptr);
  std::vector<VkQueueFamilyProperties> families(queues.Count);
  queues.Queues.resize(queues.Count);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queues.Count, families.data());
  for (U32 i = 0; i < queues.Count; i++) {
    VulkanPhysicalDeviceQueue& queue = queues.Queues[i];
    queue.Index = i;
    queue.Flags = families[i].queueFlags;
    queue.Count = families[i].queueCount;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface->GetSurface(),
                                         &queue.PresentKHR);

    // When deciding on which queue to use for what, we can simply take
    // the first graphics and presentation queue we find. In most cases,
    // they will be the same anyway.
    if (queues.GraphicsQueue == -1 && queue.SupportsGraphics()) {
      queues.GraphicsQueue = i;
    }
    if (queues.PresentQueue == -1 && queue.SupportsPresentation()) {
      queues.PresentQueue = i;
    }

    // However, when it comes to Transfer and Compute, we would prefer
    // these to be on separate queues if at all possible.
    // So, at first we simply find the first queue available. But if
    // another queue comes along that supports it and isn't our Graphics
    // queue, we will use that instead.
    if (queue.SupportsCompute() &&
        (queues.ComputeQueue == -1 || queues.ComputeQueue == queues.GraphicsQueue)) {
      queues.ComputeQueue = i;
    }
    if (queue.SupportsTransfer() &&
        (queues.TransferQueue == -1 || queues.TransferQueue == queues.GraphicsQueue)) {
      queues.TransferQueue = i;
    }
  }
  // Vulkan devices are not strictly required to advertise the Transfer
  // flag on queues that support Graphics or Compute. So, just in case,
  // if we haven't found an explicitly stated Transfer queue, we can use
  // our Graphics queue.
  if (queues.TransferQueue == -1) {
    queues.TransferQueue = queues.GraphicsQueue;
  }
}

void VulkanPhysicalDevice::QueryDeviceSwapchainSupport(VkPhysicalDevice physicalDevice,
                                                       VulkanSurface* surface,
                                                       VulkanSwapchainSupport& support) {
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface->GetSurface(),
                                            &support.Capabilities);

  U32 formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface->GetSurface(), &formatCount,
                                       nullptr);
  if (formatCount != 0) {
    support.Formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface->GetSurface(), &formatCount,
                                         support.Formats.data());
  }

  U32 presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface->GetSurface(),
                                            &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    support.PresentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface->GetSurface(),
                                              &presentModeCount, support.PresentModes.data());
  }
}

void VulkanPhysicalDevice::QueryDeviceExtensions(VkPhysicalDevice physicalDevice,
                                                 std::vector<std::string>& extensions) {
  U32 extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> properties(extensionCount);
  extensions.clear();
  extensions.reserve(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, properties.data());

  for (VkExtensionProperties& ext : properties) {
    extensions.push_back(std::string(ext.extensionName));
  }
}

const bool VulkanPhysicalDevice::VerifyDeviceCapabilities(VulkanPhysicalDeviceDetails& details) {
  // Require a Graphics and Presentation queue.
  if (details.Queues.GraphicsQueue == -1 || details.Queues.PresentQueue == -1) {
    Logger::Debug("Rejecting device: Missing graphics or presentation queue.");
    return false;
  }

  // Require all the necessary extensions.
  if (!VerifyDeviceExtensions(details, _requiredExtensions)) {
    Logger::Debug("Rejecting device: Missing required extensions.");
    return false;
  }

  // Require at least one image format and presentation mode.
  if (details.SwapchainSupport.Formats.size() == 0 ||
      details.SwapchainSupport.PresentModes.size() == 0) {
    Logger::Debug("Rejecting device: No image formats or presentation modes.");
    return false;
  }

  // Require Sample Anisotropy
  if (!details.Features.samplerAnisotropy) {
    Logger::Debug("Rejecting device: Does not support Sampler Anisotropy.");
    return false;
  }

  return true;
}

const bool VulkanPhysicalDevice::VerifyDeviceExtensions(
    VulkanPhysicalDeviceDetails& details, const std::vector<std::string>& requiredExtensions) {
  for (const std::string& ext : requiredExtensions) {
    auto it = std::find(details.Extensions.begin(), details.Extensions.end(), ext);
    if (it == details.Extensions.end()) {
      return false;
    }
  }

  return true;
}

void VulkanPhysicalDevice::DumpDeviceDetails(const VulkanPhysicalDeviceDetails& details) {
  Logger::Trace("Vulkan Device \"%s\":", details.Properties.deviceName);

  // General device details
  Logger::Trace("-- Vulkan API: %d.%d.%d", VK_VERSION_MAJOR(details.Properties.apiVersion),
                VK_VERSION_MINOR(details.Properties.apiVersion),
                VK_VERSION_PATCH(details.Properties.apiVersion));
  switch (details.Properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      Logger::Trace("-- Device Type: Dedicated");
      break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      Logger::Trace("-- Device Type: Integrated");
      break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      Logger::Trace("-- Device Type: Virtual");
      break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      Logger::Trace("-- Device Type: CPU");
      break;
    default:
      Logger::Trace("-- Device Type: Unknown");
      break;
  }
  Logger::Trace("-- Max 2D Resolution: %d", details.Properties.limits.maxImageDimension2D);

  // Memory Details
  Logger::Trace("-- Memory:");
  Logger::Trace("---- Types (%d):", details.MemoryProperties.memoryTypeCount);
  // DL = Device Local
  // HV = Host Visible
  // HC = Host Coherent
  // HH = Host Cached
  // LA = Lazily Allocated
  // PT = Protected
  // DC = Device Coherent (AMD)
  // DU = Device Uncached (AMD)
  Logger::Trace("------         / DL | HV | HC | HH | LA | PT | DC | DU \\");
  for (U32 memoryTypeIndex = 0; memoryTypeIndex < details.MemoryProperties.memoryTypeCount;
       memoryTypeIndex++) {
    const VkMemoryType& memType = details.MemoryProperties.memoryTypes[memoryTypeIndex];
    Logger::Trace("------ Heap %d: | %s | %s | %s | %s | %s | %s | %s | %s |", memType.heapIndex,
                  memType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? "DL" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ? "HV" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ? "HC" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT ? "HH" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT ? "LA" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT ? "PT" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD ? "DC" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD ? "DU" : "  ");
  }
  Logger::Trace("---- Heaps (%d):", details.MemoryProperties.memoryHeapCount);
  for (U32 memoryHeapIndex = 0; memoryHeapIndex < details.MemoryProperties.memoryHeapCount;
       memoryHeapIndex++) {
    const VkMemoryHeap& memHeap = details.MemoryProperties.memoryHeaps[memoryHeapIndex];
    // DL = Device Local
    // MI = Multi Instance
    // MI = Multi Instance (KHR)
    Logger::Trace("------ Heap %d: %.2f MiB { %s | %s | %s }", memoryHeapIndex,
                  ((F32)memHeap.size / 1024.0f / 1024.0f),
                  memHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "DL" : "  ",
                  memHeap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT ? "MI" : "  ",
                  memHeap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR ? "MK" : "  ");
  }

  Logger::Trace("-- Queue Families (%d):", details.Queues.Count);
  // GFX = Graphics
  // CMP = Compute
  // TRA = Transfer
  // SPB = Sparse Binding
  // PRT = Protected
  // PST = Presentation (KHR)
  Logger::Trace("----           / GFX  | CMP  | TRA  | SPB  | PRT  | PST  \\");
  const VulkanPhysicalDeviceQueues& queues = details.Queues;
  for (U32 queueIndex = 0; queueIndex < details.Queues.Count; queueIndex++) {
    const VulkanPhysicalDeviceQueue& queue = details.Queues.Queues[queueIndex];
    // Asterisk indicates the Queue Family has been selected for that particular queue operation.
    Logger::Trace(
        "---- Family %d: { %s%c | %s%c | %s%c | %s%c | %s%c | %s%c } (%d Queues)", queueIndex,
        queue.SupportsGraphics() ? "GFX" : "   ", queues.GraphicsQueue == queue.Index ? '*' : ' ',
        queue.SupportsCompute() ? "CMP" : "   ", queues.ComputeQueue == queue.Index ? '*' : ' ',
        queue.SupportsTransfer() ? "TRA" : "   ", queues.TransferQueue == queue.Index ? '*' : ' ',
        queue.SupportsSparseBinding() ? "SPB" : "   ", ' ',
        queue.SupportsProtected() ? "PRT" : "   ", ' ',
        queue.SupportsPresentation() ? "PST" : "   ",
        queues.PresentQueue == queue.Index ? '*' : ' ', queue.Count);
  }

  // Swapchain details
  Logger::Trace("-- Swapchain:");
  Logger::Trace("---- Image Count: %d Min / %d Max",
                details.SwapchainSupport.Capabilities.minImageCount,
                details.SwapchainSupport.Capabilities.maxImageCount);
  Logger::Trace("---- Image Size: %dx%d Min / %dx%d Max",
                details.SwapchainSupport.Capabilities.minImageExtent.width,
                details.SwapchainSupport.Capabilities.minImageExtent.height,
                details.SwapchainSupport.Capabilities.maxImageExtent.width,
                details.SwapchainSupport.Capabilities.maxImageExtent.height);
  Logger::Trace("---- Image Formats: %d", details.SwapchainSupport.Formats.size());
  Logger::Trace("---- Present Modes: %d", details.SwapchainSupport.PresentModes.size());

  // Extensions
  Logger::Trace("-- Extensions (%d):", details.Extensions.size());
  for (const std::string& ext : details.Extensions) {
    Logger::Trace("---- %s", ext.c_str());
  }
}
}  // namespace Vulkan
}  // namespace Onyx
