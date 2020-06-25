#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Onyx {
namespace Vulkan {
class VulkanSurface;

//! Describes a physical device's capabilities when relating to the swapchain.
struct VulkanSwapchainSupport {
  VkSurfaceCapabilitiesKHR Capabilities;    //!< Various flags describing the device's capabilities.
  std::vector<VkSurfaceFormatKHR> Formats;  //!< A list of device-supported image formats.
  std::vector<VkPresentModeKHR> PresentModes;  //!< A list of device-supported presentation modes.
};

//! Describes a physical device's command queue.
struct VulkanPhysicalDeviceQueue {
  U32 Index;            //!< The queue family's index.
  VkQueueFlags Flags;   //!< Flags indicating what this queue family is capable of executing.
  U32 Count;            //!< The number of queues belonging to this family.
  VkBool32 PresentKHR;  //!< A flag indicating this queue family's capability to present.

  //! Returns the queue family's Graphics support.
  const bool SupportsGraphics() const { return Flags & VK_QUEUE_GRAPHICS_BIT; }

  //! Returns the queue family's Compute support.
  const bool SupportsCompute() const { return Flags & VK_QUEUE_COMPUTE_BIT; }

  //! Returns the queue family's Transfer support.
  const bool SupportsTransfer() const { return Flags & VK_QUEUE_TRANSFER_BIT; }

  //! Returns the queue family's Sparse Binding support.
  const bool SupportsSparseBinding() const { return Flags & VK_QUEUE_SPARSE_BINDING_BIT; }

  //! Returns the queue family's Protected support.
  const bool SupportsProtected() const { return Flags & VK_QUEUE_PROTECTED_BIT; }

  //! Returns the queue family's Presentation support.
  const bool SupportsPresentation() const { return PresentKHR; }
};

//! Struct to contain information about a physical device's command queues.
struct VulkanPhysicalDeviceQueues {
  U32 Count;                                      //!< Total count of device queues.
  std::vector<VulkanPhysicalDeviceQueue> Queues;  //!< List of command queues.
  I32 GraphicsQueue;  //!< Refers to the first queue found that supports Graphics.
  I32 ComputeQueue;   //!< Refers to the first queue found that supports Compute.
  I32 TransferQueue;  //!< Refers to the first queue found that supports Transfer.
  I32 PresentQueue;   //!< Refers to the first queue found that supports Presentation.

  //! Construct a new VulkanPhysicalDeviceQueues instance.
  VulkanPhysicalDeviceQueues()
      : Count(0), GraphicsQueue(-1), ComputeQueue(-1), TransferQueue(-1), PresentQueue(-1) {}
};

//! Describes all known information about a Vulkan physical device.
struct VulkanPhysicalDeviceDetails {
  VkPhysicalDevice Device;                            //!< A handle to the device itself.
  VkPhysicalDeviceProperties Properties;              //!< The properties of the device.
  VkPhysicalDeviceFeatures Features;                  //!< The features of the device.
  VkPhysicalDeviceMemoryProperties MemoryProperties;  //!< The memory properties of the device.
  VulkanPhysicalDeviceQueues Queues;                  //!< The queues of the device.
  VulkanSwapchainSupport SwapchainSupport;            //!< The swapchain support of the device.
  std::vector<std::string> Extensions;                //!< The available extensions of the device.
};

//! Static class responsible for enumerating and choosing an appropriate physical device.
class VulkanPhysicalDevice final {
 public:
  VulkanPhysicalDevice() = delete;

  //! Enumerate our available physical devices and choose the best device for our needs.
  /*!
    \param instance The Vulkan instance to create the device for.
    \param surface The VulkanSurface this device will draw to.
    \param details An output parameter containing details of the found device.
    \return true upon finding a suitable physical device, false otherwise.
  */
  static const bool SelectPhysicalDevice(VkInstance instance, VulkanSurface* surface,
                                         VulkanPhysicalDeviceDetails* details);

 private:
  //! Retrieve all relevant information about the given physical device.
  static void QueryDeviceDetails(VkPhysicalDevice physicalDevice, VulkanSurface* surface,
                                 VulkanPhysicalDeviceDetails& details);

  //! Retrieve all of the given physical device's command queues.
  static void QueryDeviceQueues(VkPhysicalDevice physicalDevice, VulkanSurface* surface,
                                VulkanPhysicalDeviceQueues& queues);

  //! Retrieve the given physical device's swapchain support.
  static void QueryDeviceSwapchainSupport(VkPhysicalDevice physicalDevice, VulkanSurface* surface,
                                          VulkanSwapchainSupport& support);

  //! Retrieve a list of the given physical device's available extensions.
  static void QueryDeviceExtensions(VkPhysicalDevice physicalDevice,
                                    std::vector<std::string>& extensions);

  //! Verify a given physical device is capable of running our application's needs.
  static const bool VerifyDeviceCapabilities(VulkanPhysicalDeviceDetails& details);

  //! Verify a given physical device has all of the given extensions.
  static const bool VerifyDeviceExtensions(VulkanPhysicalDeviceDetails& details,
                                           const std::vector<std::string>& requiredExtensions);

  //! Log the details of a physical device to the console.
  static void DumpDeviceDetails(const VulkanPhysicalDeviceDetails& details);

  static const std::vector<std::string>
      _requiredExtensions;  //!< A list of required extensions for our Vulkan devices.
};
}  // namespace Vulkan
}  // namespace Onyx