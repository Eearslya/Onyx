#include "pch.h"

#include "Engine.h"
#include "Renderer/Renderer.h"

namespace Onyx {
Engine::Engine(Platform::IApplication* application) : _application(application) {
  if (!Renderer::Initialize(this)) {
    Logger::Fatal("Could not initialize rendering API!");
  }
}

Engine::~Engine() { Renderer::Shutdown(); }

void Engine::Run() {}

const bool Engine::OnLoop() { return true; }
}  // namespace Onyx