#include "pch.h"

#include "Renderer.h"

namespace Onyx {
Engine* Renderer::_engine = nullptr;

const bool Renderer::Initialize(Engine* engine) {
  _engine = engine;
  return true;
}

void Renderer::Shutdown() { _engine = nullptr; }

const bool Renderer::Frame() { return true; }
}  // namespace Onyx
