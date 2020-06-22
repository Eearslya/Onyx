#include "Defines.h"
#include "Types.h"

#include "Platform/VulkanPlatform.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
//
#include <vulkan/vulkan_win32.h>

VkSurfaceKHR Onyx::VulkanPlatform::CreateSurface(VkInstance instance,
                                                 void* windowHandle) {
  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{
      VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
  surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
  surfaceCreateInfo.hwnd = (HWND)windowHandle;
  VkSurfaceKHR surface;
  vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
  return surface;
}

void Onyx::VulkanPlatform::GetRequiredExtensions(
    std::vector<const char*>& requiredExtensions) {
  requiredExtensions.push_back("VK_KHR_surface");
  requiredExtensions.push_back("VK_KHR_win32_surface");
  requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}
#endif