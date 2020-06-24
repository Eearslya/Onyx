#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Onyx {
namespace Vulkan {
class VulkanSurface;

struct VulkanSwapchainSupport {
  VkSurfaceCapabilitiesKHR Capabilities;
  std::vector<VkSurfaceFormatKHR> Formats;
  std::vector<VkPresentModeKHR> PresentModes;
};

struct VulkanPhysicalDeviceQueue {
  U32 Index;
  VkQueueFlags Flags;
  U32 Count;
  VkBool32 PresentKHR;

  const bool SupportsGraphics() const { return Flags & VK_QUEUE_GRAPHICS_BIT; }
  const bool SupportsCompute() const { return Flags & VK_QUEUE_COMPUTE_BIT; }
  const bool SupportsTransfer() const { return Flags & VK_QUEUE_TRANSFER_BIT; }
  const bool SupportsSparseBinding() const { return Flags & VK_QUEUE_SPARSE_BINDING_BIT; }
  const bool SupportsProtected() const { return Flags & VK_QUEUE_PROTECTED_BIT; }
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

  VulkanPhysicalDeviceQueues()
      : Count(0), GraphicsQueue(-1), ComputeQueue(-1), TransferQueue(-1), PresentQueue(-1) {}

  //! Helper member to access Queues contained in this struct.
  VulkanPhysicalDeviceQueue& operator[](size_t index) { return Queues[index]; }
};

struct VulkanPhysicalDeviceDetails {
  VkPhysicalDevice Device;
  VkPhysicalDeviceProperties Properties;
  VkPhysicalDeviceFeatures Features;
  VkPhysicalDeviceMemoryProperties MemoryProperties;
  VulkanPhysicalDeviceQueues Queues;
  VulkanSwapchainSupport SwapchainSupport;
  std::vector<std::string> Extensions;
};

//! Represents our Vulkan device, both physical and logical.
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
               const std::vector<const char*>& requiredLayers, VulkanSurface* surface);
  ~VulkanDevice();

 private:
  //! Enumerate our available physical devices and choose the best device for our needs.
  /*!
    \return true upon finding a suitable physical device, false otherwise.
  */
  const bool SelectPhysicalDevice();
  void QueryDeviceDetails(VkPhysicalDevice physicalDevice, VulkanPhysicalDeviceDetails& details);
  void QueryDeviceQueues(VkPhysicalDevice physicalDevice, VulkanPhysicalDeviceQueues& queues);
  void QueryDeviceSwapchainSupport(VkPhysicalDevice physicalDevice,
                                   VulkanSwapchainSupport& support);
  void QueryDeviceExtensions(VkPhysicalDevice physicalDevice, std::vector<std::string>& extensions);
  const bool VerifyDeviceCapabilities(VulkanPhysicalDeviceDetails& details);
  const bool VerifyDeviceExtensions(VulkanPhysicalDeviceDetails& details,
                                    const std::vector<std::string>& requiredExtensions);

  void DumpDeviceDetails(const VulkanPhysicalDeviceDetails& details);

  VkInstance _instance;              //!< The Vulkan instance this device belongs to.
  bool _validationEnabled;           //!< Indicates whether extra validation is enabled.
  VulkanSurface* _surface;           //!< Our drawing surface.
  VkPhysicalDevice _physicalDevice;  //!< Our selected physical device.
  VulkanPhysicalDeviceDetails
      _physicalDeviceDetails;  //!< A cached copy of our chosen device's details.

  const static std::vector<std::string>
      _requiredExtensions;  //!< A list of required extensions for our Vulkan devices.
};
}  // namespace Vulkan
}  // namespace Onyx