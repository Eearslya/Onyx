#include "pch.h"

#include "OpenGLContext.h"

#include <glad/glad.h>

#include "Onyx/Core.h"

#ifdef ONYX_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#endif

namespace Onyx {
void OpenGLContext::Init() {
  OnyxInfo("Initializing OpenGL renderer...");

#ifdef ONYX_PLATFORM_WINDOWS
  GLFWwindow* window = static_cast<GLFWwindow*>(m_WindowHandle);
  glfwMakeContextCurrent(window);
  int glad = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  OnyxAssert(glad, "Failed to initialize Glad!");
#endif

  OnyxInfo("- Vendor: {}", glGetString(GL_VENDOR));
  OnyxInfo("- Renderer: {}", glGetString(GL_RENDERER));
  OnyxInfo("- Version: {}", glGetString(GL_VERSION));
}

void OpenGLContext::SwapBuffers() {
#ifdef ONYX_PLATFORM_WINDOWS
  GLFWwindow* window = static_cast<GLFWwindow*>(m_WindowHandle);
  glfwSwapBuffers(window);
#endif
}
}  // namespace Onyx