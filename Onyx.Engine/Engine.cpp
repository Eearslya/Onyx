#include "Engine.h"

#include "Logger.h"

namespace Onyx {
Engine::Engine(IApplication* application) {
  Logger::Info("Initializing Onyx...");
  _application = application;
}

Engine::~Engine() {}

void Engine::Run() {}

const bool Engine::OnLoop(const F32 deltaTime) { return false; }

void Engine::Shutdown() {}
}  // namespace Onyx