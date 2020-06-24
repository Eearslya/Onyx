#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

namespace Onyx {
namespace Vulkan {
//! Handles Vulkan debug callbacks.
class VulkanDebugger final {
 public:
  //! Possibly logging levels from most severe to least.
  enum class Level { ERROR = 0, WARNING, INFO, TRACE };

  //! Construct a new debugger for the given instance.
  /*!
    \param instance The parent Vulkan instance.
    \param level The minimum level of message severity to show.
  */
  VulkanDebugger(VkInstance instance, Level level);
  ~VulkanDebugger();

  //! Populate and return creation info for a debug messenger.
  /*!
    \param level The minimum level of message severity to show.
  */
  static VkDebugUtilsMessengerCreateInfoEXT GetCreateInfo(Level level);

 private:
  VkInstance _instance;                      //!< Our parent Vulkan instance.
  VkDebugUtilsMessengerEXT _debugMessenger;  //!< Our debug messenger handle.
};
}  // namespace Vulkan
}  // namespace Onyx