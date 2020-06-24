#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include "Renderer/IRendererBackend.h"

namespace Onyx {
class IApplication;

namespace Vulkan {
//! IRendererBackend implementation for Vulkan.
class VulkanRendererBackend final : public IRendererBackend {
 public:
  //! Construct a new Vulkan renderer.
  /*!
    \param application The application this renderer belongs to.
    \param enableValidation Indicates whether to enable validation layers.
  */
  VulkanRendererBackend(IApplication* application, const bool enableValidation);
  ~VulkanRendererBackend();

  const bool ValidationEnabled() const override { return _validationEnabled; }

 private:
  IApplication* _application;  //!< Our parent application.
  bool _validationEnabled;     //!< Whether validation is enabled or not.
};
}  // namespace Vulkan
}  // namespace Onyx