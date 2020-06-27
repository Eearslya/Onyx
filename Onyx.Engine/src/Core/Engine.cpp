#include "pch.h"

#include "Engine.h"

#include "Renderer/Renderer.h"

namespace Onyx {
void Engine::Initialize() {
  if (!Renderer::Initialize()) {
    Logger::Fatal("Could not initialize rendering API!");
  }
}

void Engine::Shutdown() { Renderer::Shutdown(); }

void Engine::Run() {}

const bool Engine::OnLoop() {
  Renderer::PrepareFrame();
  Renderer::Frame();
  return true;
}
}  // namespace Onyx