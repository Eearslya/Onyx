#pragma once

#include "Onyx/Renderer/GraphicsContext.h"

namespace Onyx {
class OpenGLContext : public GraphicsContext {
 public:
  OpenGLContext::OpenGLContext(void* handle) : m_WindowHandle(handle) {}

  void Init() override;
  void Shutdown() override {}
  void SwapBuffers() override;

 private:
  void* m_WindowHandle;
};
}  // namespace Onyx