#include "pch.h"

#include "VulkanDebugger.h"

#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
static VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebuggerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      Logger::Error("Vulkan ERROR: %s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      Logger::Warn("Vulkan WARNING: %s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      Logger::Info("Vulkan INFO: %s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      Logger::Trace("Vulkan VERBOSE: %s", pCallbackData->pMessage);
      break;
  }

  return VK_FALSE;
}

VulkanDebugger::VulkanDebugger(VkInstance instance, Level level)
    : _instance(instance), _debugMessenger(VK_NULL_HANDLE) {
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = GetCreateInfo(level);
  debugCreateInfo.pUserData = this;

  static PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance,
                                                                "vkCreateDebugUtilsMessengerEXT");
  ASSERT_MSG(func, "Could not locate vkCreateDebugUtilsMessengerEXT!");
  VK_CHECK(func(_instance, &debugCreateInfo, nullptr, &_debugMessenger));
}

VulkanDebugger::~VulkanDebugger() {
  static PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance,
                                                                 "vkDestroyDebugUtilsMessengerEXT");
  if (func) {  // Must not throw/assert during destruction
    func(_instance, _debugMessenger, nullptr);
  }
}

VkDebugUtilsMessengerCreateInfoEXT VulkanDebugger::GetCreateInfo(Level level) {
  U32 severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  if (level >= Level::WARNING) {
    severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  }
  if (level >= Level::INFO) {
    severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  }
  if (level >= Level::TRACE) {
    severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  }

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
  debugCreateInfo.messageSeverity = severity;
  debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  debugCreateInfo.pfnUserCallback = VulkanDebuggerCallback;

  return debugCreateInfo;
}
}  // namespace Vulkan
}  // namespace Onyx
