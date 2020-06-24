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
class VulkanDebugger;
class VulkanSurface;

//! IRendererBackend implementation for Vulkan.
class VulkanRendererBackend final : public IRendererBackend {
 public:
  //! Construct a new Vulkan renderer.
  /*!
    \param application The application this renderer belongs to.
    \param enableValidation Indicates whether to enable validation layers.
  */
  VulkanRendererBackend(Platform::IApplication* application, const bool enableValidation);
  ~VulkanRendererBackend();

  const bool ValidationEnabled() const override { return _validationEnabled; }

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

  VkInstance _instance = VK_NULL_HANDLE;  //!< Our Vulkan instance.
  VulkanDebugger* _debugger = nullptr;
  VulkanSurface* _surface = nullptr;
};
}  // namespace Vulkan
}  // namespace Onyx