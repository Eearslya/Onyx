#include "pch.h"

#include "Renderer.h"

#include "Core/Engine.h"
#include "Renderer/Vulkan/VulkanRendererBackend.h"

namespace Onyx {
Engine* Renderer::_engine = nullptr;
IRendererBackend* Renderer::_backend = nullptr;

const bool Renderer::Initialize(Engine* engine) {
  _engine = engine;

  _backend = new Vulkan::VulkanRendererBackend(_engine->GetApplication(), true);
  return true;
}

void Renderer::Shutdown() {
  delete _backend;
  _engine = nullptr;
}

const bool Renderer::PrepareFrame() { return _backend->PrepareFrame(); }

const bool Renderer::Frame() { return _backend->Frame(); }
}  // namespace Onyx
