#include "pch.h"

#include "Core/Defines.h"
#include "Core/Types.h"

#include "Platform/Application.h"
#include "Platform/VulkanPlatform.h"

#ifdef ONYX_PLATFORM_WINDOWS
#include <Windows.h>
//
#include <vulkan/vulkan_win32.h>

namespace Onyx {
namespace Vulkan {
VkSurfaceKHR VulkanPlatform::CreateSurface(VulkanRenderer* backend) {
  Logger::Debug("Creating Win32 Vulkan surface");
  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
  surfaceCreateInfo.hinstance = static_cast<HINSTANCE>(Application::GetHandle());
  surfaceCreateInfo.hwnd = static_cast<HWND>(Application::GetWindowHandle());

  VkSurfaceKHR surface;
  vkCreateWin32SurfaceKHR(backend->GetInstance(), &surfaceCreateInfo, nullptr, &surface);
  return surface;
}

void VulkanPlatform::GetRequiredExtensions(std::vector<const char*>& requiredExtensions) {
  requiredExtensions.push_back("VK_KHR_surface");
  requiredExtensions.push_back("VK_KHR_win32_surface");
  requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}
}  // namespace Vulkan
}  // namespace Onyx
#endif