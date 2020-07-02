#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

#include <array>
#include <chrono>
#include <vector>

#include "RendererTypes.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Onyx {
class Mesh;

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
  VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
  VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
  VkPipeline GraphicsPipeline = VK_NULL_HANDLE;
  std::vector<VkFramebuffer> SwapchainFramebuffers;
  VkCommandPool GraphicsCommandPool = VK_NULL_HANDLE;
  VkCommandPool TransferCommandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> GraphicsCommandBuffers;
  std::vector<VkSemaphore> ImageAvailableSemaphores;
  std::vector<VkSemaphore> RenderFinishedSemaphores;
  std::vector<VkFence> InFlightFences;
  std::vector<VkFence> ImagesInFlight;
  U32 CurrentFrame = 0;
  std::vector<VkBuffer> UniformBuffers;
  std::vector<VkDeviceMemory> UniformBufferMemories;
  VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> DescriptorSets;
  VkImage TextureImage = VK_NULL_HANDLE;
  VkDeviceMemory TextureImageMemory = VK_NULL_HANDLE;
  VkImageView TextureImageView = VK_NULL_HANDLE;
  VkSampler TextureSampler = VK_NULL_HANDLE;
};

class Renderer final {
 public:
  static const bool Initialize();
  static void Shutdown();
  static const bool Frame();

  static void UploadMesh(Mesh& mesh);

  static const bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                 VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                 VkDeviceMemory& deviceMemory, U64* alignment = nullptr);
  static void CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size);
  static void CopyBufferToImage(VkBuffer source, VkImage destination, U32 width, U32 height);
  static const bool CreateImage(U32 width, U32 height, VkFormat format, VkImageTiling tiling,
                                VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                                VkImage& image, VkDeviceMemory& memory);

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
  static const bool CreateDescriptorSetLayout();
  static const bool CreateGraphicsPipeline();
  static VkShaderModule CreateShaderModule(const std::vector<char>& source);
  static const bool CreateFramebuffers();
  static const bool CreateUniformBuffers();
  static const bool CreateDescriptorPool();
  static const bool CreateDescriptorSets();
  static const bool AllocateGraphicsCommandBuffers();
  static const bool CreateTextureImage();
  static const bool CreateTextureImageView();
  static const bool CreateTextureSampler();

  static const bool RecreateSwapchain();

  // Object destruction
  static void DestroyTextureSampler();
  static void DestroyTextureImageView();
  static void DestroyTextureImage();
  static void DestroySyncObjects();
  static void DestroyDescriptorSets();
  static void DestroyDescriptorPool();
  static void DestroyUniformBuffers();
  static void FreeGraphicsCommandBuffers();
  static void DestroyCommandPools();
  static void DestroyFramebuffers();
  static void DestroyShaderModule(VkShaderModule module);
  static void DestroyDescriptorSetLayout();
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
  static void BeginCommandBuffer(VkCommandBuffer buffer, bool transient = false);
  static void BeginRenderPass(VkCommandBuffer buffer, VkFramebuffer framebuffer);
  static void BindGraphicsPipeline(VkCommandBuffer buffer);
  static void BindVertexBuffers(VkCommandBuffer buffer, std::vector<VkBuffer> buffers,
                                std::vector<U64> offsets);
  static void BindIndexBuffer(VkCommandBuffer buffer, VkBuffer indexBuffer, U32 firstIndex);
  static void BindDescriptorSets(VkCommandBuffer buffer,
                                 const std::vector<VkDescriptorSet>& descriptorSets, U32 firstSet);
  static void Draw(VkCommandBuffer buffer, U32 vertexCount, U32 instanceCount, U32 firstVertex,
                   U32 firstInstance);
  static void DrawIndexed(VkCommandBuffer buffer, U32 indexCount, U32 instanceCount, U32 firstIndex,
                          U32 indexOffset, U32 firstInstance);
  static void EndRenderPass(VkCommandBuffer buffer);
  static void EndCommandBuffer(VkCommandBuffer buffer);
  static U32 FindMemoryType(U32 typeFilter, VkMemoryPropertyFlags properties);
  static void UpdateUniformBuffer(U32 imageIndex);
  static VkCommandBuffer BeginSingleCommandBuffer();
  static void EndSingleCommandBuffer(VkCommandBuffer buffer);
  static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout srcLayout,
                                    VkImageLayout dstLayout);
};
}  // namespace Onyx