#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer/IRendererBackend.h"

namespace Onyx {
namespace Platform {
class IApplication;
}

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

//! IRendererBackend implementation for Vulkan.
class VulkanRendererBackend final : public IRendererBackend {
 public:
  //! Construct a new Vulkan renderer.
  /*!
    \param application The application this renderer belongs to.
    \param enableValidation Indicates whether to enable validation layers.
  */
  VulkanRendererBackend(const bool enableValidation);
  ~VulkanRendererBackend();

  const bool PrepareFrame() override;
  const bool Frame() override;

  //! Get our Vulkan instance.
  /*!
    \return The Vulkan instance.
  */
  VkInstance GetInstance() { return _instance; }

  //! Get our parent application.
  /*!
    \return The parent application.
  */
  Platform::IApplication* GetApplication() { return _application; }

 private:
  //! Create our Vulkan instance.
  const bool CreateInstance();

  //! Verify that the instance can support all extensions given.
  const bool VerifyInstanceExtensions(const std::vector<const char*>& requiredExtensions);

  //! Verify that the instance can support all layers given.
  const bool VerifyInstanceLayers(const std::vector<const char*>& requiredLayers);

  //! Destroy and cleanup our Vulkan instance.
  void DestroyInstance();

  Platform::IApplication* _application;  //!< Our parent application.
  bool _validationEnabled;               //!< Whether validation is enabled or not.
  std::vector<const char*>
      _requiredExtensions;                   //!< A list of extensions required to run our renderer.
  std::vector<const char*> _requiredLayers;  //!< A list of layers required to run our renderer.

  VkInstance _instance = VK_NULL_HANDLE;              //!< Our Vulkan instance.
  VulkanDebugger* _debugger = nullptr;                //!< Our Vulkan debugger.
  VulkanSurface* _surface = nullptr;                  //!< Our drawing surface.
  VulkanDevice* _device = nullptr;                    //!< Our Vulkan device.
  VulkanSwapchain* _swapchain = nullptr;              //!< Our swapchain.
  VulkanRenderPass* _renderPass = nullptr;            //!< Our default render pass.
  std::vector<VulkanCommandBuffer*> _commandBuffers;  //!< Our command buffers.
  std::vector<VulkanSemaphore*>
      _imageAvailableSemaphores;  //!< Signals when swapchain image is available.
  std::vector<VulkanSemaphore*> _renderFinishedSemaphores;  //!< Signals when rendering complete.
  std::vector<VulkanFence*>
      _inFlightFences;  //!< Fences to keep us from processing too many frames at once.
  std::vector<VulkanFence*>
      _imagesInFlight;  //!< Fences to keep us from using the same image at the same time.

  U32 _currentImageIndex;  //!< Current index into our swapchain.
  U32 _currentFrame;

  VulkanShader* _shader = nullptr;  //!< TEMPORARY Shader module.

  static const U32 _maxFramesInFlight = 2;
};
}  // namespace Vulkan
}  // namespace Onyx