#include "RendererFrontend.h"

#include "Engine.h"
#include "Logger.h"
#include "Renderer/Backend/Vulkan/VulkanRendererBackend.h"

namespace Onyx {
Engine* RendererFrontEnd::_engine;
IRendererBackend* RendererFrontEnd::_backend;

const bool RendererFrontEnd::Initialize(Engine* engine) {
  _engine = engine;
  const bool validationEnabled = true;
  Logger::Debug("Created Renderer front-end. Renderer validation %s.",
                validationEnabled ? "enabled" : "disabled");

  _backend = new VulkanRendererBackend(_engine->GetApplication());
  _backend->Initialize(validationEnabled);

  return true;
}

void RendererFrontEnd::Shutdown() {
  if (_backend) {
    _backend->Shutdown();
    delete _backend;
  }
}
}  // namespace Onyx