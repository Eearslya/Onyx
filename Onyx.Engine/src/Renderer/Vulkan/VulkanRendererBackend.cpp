#include "pch.h"

#include "VulkanRendererBackend.h"

namespace Onyx {
namespace Vulkan {
VulkanRendererBackend::VulkanRendererBackend(IApplication* application, const bool enableValidation)
    : IRendererBackend(application, enableValidation),
      _application(application),
      _validationEnabled(enableValidation) {}

VulkanRendererBackend::~VulkanRendererBackend() {}
}  // namespace Vulkan
}  // namespace Onyx