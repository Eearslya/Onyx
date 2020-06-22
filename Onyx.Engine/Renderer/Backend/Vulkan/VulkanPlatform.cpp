#include "VulkanPlatform.h"

#include "Defines.h"
#include "Types.h"

void Onyx::VulkanPlatform::CreateSurface(void* windowHandle,
                                         VkInstance instance,
                                         VkSurfaceKHR* outSurface) {}

void Onyx::VulkanPlatform::GetRequiredExtensions(
    std::vector<const char*>& requiredExtensions) {
  requiredExtensions.push_back("VK_KHR_surface");
#ifdef PLATFORM_WINDOWS
  requiredExtensions.push_back("VK_KHR_win32_surface");
#endif
  requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}
