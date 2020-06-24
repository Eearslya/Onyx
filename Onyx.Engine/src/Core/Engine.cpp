#include "pch.h"

#include "Engine.h"

namespace Onyx {
Engine::Engine(Platform::IApplication* application) : _application(application) {}

Engine::~Engine() {}

void Engine::Run() {}

const bool Engine::OnLoop() { return true; }
}  // namespace Onyx