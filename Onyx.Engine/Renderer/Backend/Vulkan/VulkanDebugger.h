#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
class VulkanDebugger final {
 public:
  enum class Level { ERROR = 0, WARNING, INFO, TRACE };

  const bool Initialize(VkInstance instance, Level level);
  void Shutdown();

  static VkDebugUtilsMessengerCreateInfoEXT GetCreateInfo(Level level);

 private:
  VkInstance _instance;
  VkDebugUtilsMessengerEXT _debugMessenger;
};
}  // namespace Onyx