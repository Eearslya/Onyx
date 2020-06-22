#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer/Backend/IRendererBackend.h"

namespace Onyx {
class IApplication;

class VulkanDebugger;

class VulkanRendererBackend final : public IRendererBackend {
 public:
  VulkanRendererBackend(IApplication* application);
  ~VulkanRendererBackend();

  const bool Initialize(const bool enableValidation) override;
  void Shutdown() override;
  const bool PrepareFrame(const F32 deltaTime) override;
  const bool Frame(const F32 deltaTime) override;

  const bool ValidationEnabled() const override;
  const bool IsShutdown() const override;

 private:
  void CreateInstance();

  IApplication* _application;
  bool _validationEnabled;

  std::vector<const char*> _requiredLayers;
  VkInstance _instance = nullptr;
  VulkanDebugger* _debugger;
};
}  // namespace Onyx