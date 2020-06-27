#include "pch.h"

#include "Renderer.h"

#include "Core/Engine.h"
#include "Renderer/Vulkan/VulkanRenderer.h"

namespace Onyx {
IRenderer* Renderer::_backend = nullptr;

const bool Renderer::Initialize() {
  _backend = new Vulkan::VulkanRenderer(true);
  return true;
}

void Renderer::Shutdown() { delete _backend; }

const bool Renderer::PrepareFrame() { return _backend->PrepareFrame(); }

const bool Renderer::Frame() { return _backend->Frame(); }
}  // namespace Onyx
