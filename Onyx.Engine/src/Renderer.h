#pragma once

#include "Defines.h"
#include "Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
class Renderer final {
 public:
  static const bool Initialize();
  static void Shutdown();
  static const bool PrepareFrame();
  static const bool Frame();

 private:
  // Object creation
  static const bool CreateInstance();
  static const bool CreateDebugMessenger();

  // Object destruction
  static void DestroyDebugMessenger();
  static void DestroyInstance();

  // Helper methods
  static const bool ValidateInstanceExtensions(const std::vector<const char*> extensions);
  static const bool ValidateInstanceLayers(const std::vector<const char*> layers);
  static void FillDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};
}  // namespace Onyx