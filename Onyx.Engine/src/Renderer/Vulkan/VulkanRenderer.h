#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer/IRenderer.h"

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

  VkInstance GetInstance() { return _instance; }

 private:
  const bool CreateInstance();
  const bool VerifyInstanceExtensions(const std::vector<const char*>& requiredExtensions);
  const bool VerifyInstanceLayers(const std::vector<const char*>& requiredLayers);
  const bool CreateDebugger(DebuggerLevel level);
  void DestroyDebugger();
  void DestroyInstance();

  void FillDebuggerInfo(DebuggerLevel level, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);

  bool _validationEnabled;
  std::vector<const char*> _requiredExtensions;
  std::vector<const char*> _requiredLayers;

  VkInstance _instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT m_Debugger = VK_NULL_HANDLE;
  VulkanSurface* _surface = nullptr;
  VulkanDevice* _device = nullptr;
  VulkanSwapchain* _swapchain = nullptr;
  VulkanRenderPass* _renderPass = nullptr;
  std::vector<VulkanCommandBuffer*> _commandBuffers;
  std::vector<VulkanSemaphore*> _imageAvailableSemaphores;
  std::vector<VulkanSemaphore*> _renderFinishedSemaphores;
  std::vector<VulkanFence*> _inFlightFences;
  std::vector<VulkanFence*> _imagesInFlight;

  U32 _currentImageIndex;
  U32 _currentFrame;

  VulkanShader* _shader = nullptr;  //!< TEMPORARY Shader module.

  static const U32 _maxFramesInFlight = 2;
};
}  // namespace Vulkan
}  // namespace Onyx