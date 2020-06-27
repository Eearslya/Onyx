#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer/IRenderer.h"
#include "Renderer/Vertex.h"
#include "Renderer/Vulkan/VulkanBuffer.h"

namespace Onyx {
namespace Vulkan {
class VulkanCommandBuffer;
class VulkanDebugger;
class VulkanDevice;
class VulkanFence;
class VulkanRenderPass;
class VulkanSemaphore;
class VulkanShader;
class VulkanSurface;
class VulkanSwapchain;

class VulkanRenderer final : public IRenderer {
 public:
  enum class DebuggerLevel { Error = 0, Warning, Info, Trace };

  VulkanRenderer(const bool enableValidation);
  ~VulkanRenderer();

  const bool PrepareFrame() override;
  const bool Frame() override;

  VkInstance GetInstance() { return m_Instance; }

 private:
  const bool CreateInstance();
  const bool VerifyInstanceExtensions(const std::vector<const char*>& requiredExtensions);
  const bool VerifyInstanceLayers(const std::vector<const char*>& requiredLayers);
  const bool CreateDebugger(DebuggerLevel level);
  void DestroyDebugger();
  void DestroyInstance();

  void FillDebuggerInfo(DebuggerLevel level, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);

  bool _validationEnabled;
  std::vector<const char*> m_RequiredExtensions;
  std::vector<const char*> m_RequiredLayers;

  VkInstance m_Instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT m_Debugger = VK_NULL_HANDLE;
  VulkanSurface* m_Surface = nullptr;
  VulkanDevice* m_Device = nullptr;
  VulkanSwapchain* m_Swapchain = nullptr;
  VulkanRenderPass* m_RenderPass = nullptr;
  std::vector<VulkanCommandBuffer*> m_CommandBuffers;
  std::vector<VulkanSemaphore*> m_ImageAvailableSemaphores;
  std::vector<VulkanSemaphore*> m_RenderFinishedSemaphores;
  std::vector<VulkanFence*> m_InFlightFences;
  std::vector<VulkanFence*> m_ImagesInFlight;

  U32 m_CurrentImageIndex;
  U32 m_CurrentFrame;

  // Temporary for testing.
  VulkanShader* m_Shader = nullptr;
  std::vector<Vertex> m_Vertices;
  VulkanVertexBuffer m_VertexBuffer;
  VulkanIndexBuffer m_IndexBuffer;

  static const U32 s_MaxFramesInFlight = 2;
};
}  // namespace Vulkan
}  // namespace Onyx