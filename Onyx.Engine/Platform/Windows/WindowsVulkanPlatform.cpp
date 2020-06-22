#include "Defines.h"
#include "Types.h"

#include "Platform/VulkanPlatform.h"

#ifdef PLATFORM_WINDOWS
void Onyx::VulkanPlatform::CreateSurface(void* windowHandle,
                                         VkInstance instance,
                                         VkSurfaceKHR* outSurface) {}

void Onyx::VulkanPlatform::GetRequiredExtensions(
    std::vector<const char*>& requiredExtensions) {
  requiredExtensions.push_back("VK_KHR_surface");
  requiredExtensions.push_back("VK_KHR_win32_surface");
  requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}
#endif