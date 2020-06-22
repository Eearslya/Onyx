#pragma once

#include "Defines.h"
#include "Types.h"

namespace Onyx {
class Engine;
class IRendererBackend;

class RendererFrontEnd final {
 public:
  static const bool Initialize(Engine* engine);
  static void Shutdown();
  static const bool Frame(const F32 deltaTime);

 private:
  RendererFrontEnd() noexcept {}
  ~RendererFrontEnd() noexcept {}

  static Engine* _engine;
  static IRendererBackend* _backend;
};
}  // namespace Onyx