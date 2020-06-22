#include "Engine.h"

#include "Logger.h"
#include "Renderer/Frontend/RendererFrontend.h"

namespace Onyx {
Engine::Engine(IApplication* application) : _application(application) {
  Logger::Info("Initializing Onyx...");
}

Engine::~Engine() {}

void Engine::Run() {
  if (!RendererFrontEnd::Initialize(this)) {
    Logger::Fatal("Failed to initialize renderer!");
    ASSERT(false);
  }
}

const bool Engine::OnLoop(const F32 deltaTime) { return false; }

void Engine::Shutdown() { RendererFrontEnd::Shutdown(); }
}  // namespace Onyx