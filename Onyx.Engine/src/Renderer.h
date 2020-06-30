#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
struct VulkanPhysicalDeviceSwapchainSupport {
  VkSurfaceCapabilitiesKHR Capabilities;
  std::vector<VkSurfaceFormatKHR> Formats;
  std::vector<VkPresentModeKHR> PresentationModes;
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

struct VulkanPhysicalDeviceQueues {
  std::vector<VulkanPhysicalDeviceQueue> Queues;

  U32 Count;
  I32 GraphicsIndex;
  I32 TransferIndex;
  I32 ComputeIndex;
  I32 PresentationIndex;
};

struct VulkanPhysicalDeviceInfo {
  VkPhysicalDevice Device;
  VkPhysicalDeviceFeatures Features;
  VkPhysicalDeviceMemoryProperties Memory;
  VkPhysicalDeviceProperties Properties;
  VulkanPhysicalDeviceQueues Queues;
  VulkanPhysicalDeviceSwapchainSupport SwapchainSupport;
  std::vector<VkExtensionProperties> Extensions;
};

struct VulkanContext {
  VkInstance Instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
  VkSurfaceKHR Surface = VK_NULL_HANDLE;
  std::vector<VulkanPhysicalDeviceInfo> PhysicalDevices;
  VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
  VulkanPhysicalDeviceInfo PhysicalDeviceInfo;
  VkDevice Device = VK_NULL_HANDLE;
  VkQueue GraphicsQueue = VK_NULL_HANDLE;
  VkQueue PresentationQueue = VK_NULL_HANDLE;
  VkQueue TransferQueue = VK_NULL_HANDLE;
  VkSurfaceFormatKHR SwapchainSurfaceFormat;
  VkPresentModeKHR SwapchainPresentMode;
  VkExtent2D SwapchainExtent;
  U32 SwapchainImageCount;
  VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
  std::vector<VkImage> SwapchainImages;
  std::vector<VkImageView> SwapchainImageViews;
  VkRenderPass RenderPass = VK_NULL_HANDLE;
  VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
  VkPipeline GraphicsPipeline = VK_NULL_HANDLE;
  std::vector<VkFramebuffer> SwapchainFramebuffers;
  VkCommandPool GraphicsCommandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> GraphicsCommandBuffers;
  std::vector<VkSemaphore> ImageAvailableSemaphores;
  std::vector<VkSemaphore> RenderFinishedSemaphores;
  std::vector<VkFence> InFlightFences;
  std::vector<VkFence> ImagesInFlight;
  U32 CurrentFrame = 0;
};

class Renderer final {
 public:
  static const bool Initialize();
  static void Shutdown();
  static const bool Frame();

 private:
  // Object creation
  static const bool CreateInstance();
  static const bool CreateDebugMessenger();
  static const bool CreateSurface();
  static const bool CreateDevice();
  static const bool CreateCommandPools();
  static const bool CreateSyncObjects();
  static const bool CreateSwapchainObjects();
  static const bool CreateSwapchain();
  static const bool CreateSwapchainImages();
  static const bool CreateRenderPass();
  static const bool CreateGraphicsPipeline();
  static VkShaderModule CreateShaderModule(const std::vector<char>& source);
  static const bool CreateFramebuffers();
  static const bool AllocateGraphicsCommandBuffers();

  static const bool RecreateSwapchain();

  // Object destruction
  static void DestroySyncObjects();
  static void FreeGraphicsCommandBuffers();
  static void DestroyCommandPools();
  static void DestroyFramebuffers();
  static void DestroyShaderModule(VkShaderModule module);
  static void DestroyGraphicsPipeline();
  static void DestroyRenderPass();
  static void DestroySwapchainImages();
  static void DestroySwapchain();
  static void DestroySwapchainObjects();
  static void DestroyDevice();
  static void DestroySurface();
  static void DestroyDebugMessenger();
  static void DestroyInstance();

  // Helper methods
  static const bool ValidateInstanceExtensions(const std::vector<const char*> extensions);
  static const bool ValidateInstanceLayers(const std::vector<const char*> layers);
  static void FillDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  static const bool SelectPhysicalDevice();
  static void QueryPhysicalDeviceInfo(VkPhysicalDevice device,
                                      VulkanPhysicalDeviceInfo& deviceInfo);
  static void QueryPhysicalDeviceQueues(VkPhysicalDevice device,
                                        VulkanPhysicalDeviceQueues& queueInfo);
  static void QueryPhysicalDeviceSwapchainSupport(VkPhysicalDevice device,
                                                  VulkanPhysicalDeviceSwapchainSupport& support);
  static void QueryPhysicalDeviceExtensions(VkPhysicalDevice device,
                                            std::vector<VkExtensionProperties>& extensions);
  static const bool ValidatePhysicalDevice(VulkanPhysicalDeviceInfo& deviceInfo);
  static void DumpPhysicalDeviceInfo(const VulkanPhysicalDeviceInfo& info);
  static const bool GetDeviceQueues();
  static const bool GetSwapchainSurfaceFormat();
  static const bool GetSwapchainPresentMode();
  static const bool GetSwapchainExtent();
  static void BeginCommandBuffer(VkCommandBuffer buffer);
  static void BeginRenderPass(VkCommandBuffer buffer, VkFramebuffer framebuffer);
  static void BindGraphicsPipeline(VkCommandBuffer buffer);
  static void Draw(VkCommandBuffer buffer, U32 vertexCount, U32 instanceCount, U32 firstVertex,
                   U32 firstInstance);
  static void EndRenderPass(VkCommandBuffer buffer);
  static void EndCommandBuffer(VkCommandBuffer buffer);
};
}  // namespace Onyx