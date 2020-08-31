#include "pch.h"

#include <GLFW/glfw3.h>

#include "Onyx/Application.h"
#include "Onyx/Core.h"
#include "Onyx/Input.h"
#include "Onyx/Window.h"

#ifdef ONYX_PLATFORM_WINDOWS
namespace Onyx {
bool Input::IsKeyPressed(Key keycode) {
  auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
  int keyState = glfwGetKey(window, static_cast<int>(keycode));

  return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
}

bool Input::IsMousePressed(MouseButton button) {
  auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());

  return glfwGetMouseButton(window, static_cast<int>(button)) == GLFW_PRESS;
}

void Input::GetMousePos(float* x, float* y) {
  auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
  double dX, dY;
  glfwGetCursorPos(window, &dX, &dY);

  *x = static_cast<float>(dX);
  *y = static_cast<float>(dY);
}

float Input::GetMouseX() {
  auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
  double dX, dY;
  glfwGetCursorPos(window, &dX, &dY);

  return static_cast<float>(dX);
}

float Input::GetMouseY() {
  auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
  double dX, dY;
  glfwGetCursorPos(window, &dX, &dY);

  return static_cast<float>(dY);
}
}  // namespace Onyx
#endif /* ONYX_PLATFORM_WINDOWS */