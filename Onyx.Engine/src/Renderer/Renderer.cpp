#include "pch.h"

#include "Renderer.h"

#include "Core/Engine.h"
#include "Renderer/Vulkan/VulkanRendererBackend.h"

namespace Onyx {
IRendererBackend* Renderer::_backend = nullptr;

const bool Renderer::Initialize() {
  _backend = new Vulkan::VulkanRendererBackend(true);
  return true;
}

void Renderer::Shutdown() {
  delete _backend;
}

const bool Renderer::PrepareFrame() { return _backend->PrepareFrame(); }

const bool Renderer::Frame() { return _backend->Frame(); }
}  // namespace Onyx
