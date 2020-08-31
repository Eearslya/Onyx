#pragma once

namespace Onyx {
class GraphicsContext {
 public:
  virtual void Init() = 0;
  virtual void Shutdown() = 0;
  virtual void SwapBuffers() = 0;
};
}  // namespace Onyx