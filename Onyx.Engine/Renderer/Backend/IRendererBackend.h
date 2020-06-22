#pragma once

#include "Defines.h"
#include "Types.h"

namespace Onyx {
class IRendererBackend {
 public:
  virtual ~IRendererBackend() {}

  virtual const bool Initialize(const bool enableValidation) = 0;
  virtual void Shutdown() = 0;
  virtual const bool PrepareFrame(const F32 deltaTime) = 0;
  virtual const bool Frame(const F32 deltaTime) = 0;

  virtual const bool ValidationEnabled() const = 0;
  virtual const bool IsShutdown() const = 0;
};
}  // namespace Onyx