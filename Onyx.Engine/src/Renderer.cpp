#include "Renderer.h"

#include <vulkan/vulkan.h>

#include "Application.h"
#include "Logger.h"

#define VkCall(result) \
  { ASSERT(result == VK_SUCCESS); }

namespace Onyx {
static struct VulkanContext {
  VkInstance Instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
} vkContext;

static const std::vector<const char*> g_RequiredInstanceExtensions = {"VK_KHR_surface",
                                                                      "VK_KHR_win32_surface"};
static const std::vector<const char*> g_RequiredInstanceValidationExtensions = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
static const std::vector<const char*> g_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef ONYX_DEBUG
static const bool g_EnableValidation = true;
#else
static const bool g_EnableValidation = false;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      Logger::Error("%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      Logger::Warn("%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      Logger::Info("%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      Logger::Trace("%s", pCallbackData->pMessage);
      break;
  }

  return VK_FALSE;
}

const bool Renderer::Initialize() {
  Logger::Info("Initializing renderer...");

  ASSERT(CreateInstance());
  if (g_EnableValidation) {
    ASSERT(CreateDebugMessenger());
  }

  Logger::Info("Renderer finished initialization.");
  return true;
}

void Renderer::Shutdown() {
  Logger::Info("Renderer shutting down.");
  DestroyDebugMessenger();
  DestroyInstance();
}

const bool Renderer::PrepareFrame() { return true; }

const bool Renderer::Frame() { return true; }

const bool Renderer::CreateInstance() {
  Logger::Trace("Creating Vulkan instance.");

  VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.pApplicationName = "Onyx";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Onyx";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo createInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  createInfo.pApplicationInfo = &appInfo;

  VkDebugUtilsMessengerCreateInfoEXT instanceDebugMessenger{};

  // Determine what instance extensions are required for our functionality.
  // First, use our global extension list as a base.
  std::vector<const char*> requiredExtensions = g_RequiredInstanceExtensions;
  // If we want to enable validation, we need the validation extensions as well.
  if (g_EnableValidation) {
    if (ValidateInstanceLayers(g_ValidationLayers)) {
      Logger::Debug("Loading Vulkan validation layers.");
      createInfo.enabledLayerCount = static_cast<U32>(g_ValidationLayers.size());
      createInfo.ppEnabledLayerNames = g_ValidationLayers.data();
    } else {
      Logger::Fatal("Validation layers requested, but were not available.");
      return false;
    }

    requiredExtensions.insert(requiredExtensions.end(),
                              g_RequiredInstanceValidationExtensions.begin(),
                              g_RequiredInstanceValidationExtensions.end());
    FillDebugMessengerInfo(instanceDebugMessenger);
    createInfo.pNext = &instanceDebugMessenger;
  }
  // Verify we have all of the extensions available to us.
  if (!ValidateInstanceExtensions(requiredExtensions)) {
    Logger::Fatal("Could not load required Vulkan instance extensions!");
    return false;
  }
  Logger::Trace("Loading %d instance extensions:", requiredExtensions.size());
  for (U32 i = 0; i < requiredExtensions.size(); i++) {
    Logger::Trace(" - %s", requiredExtensions[i]);
  }
  // Finally, set the required extensions to load.
  createInfo.enabledExtensionCount = static_cast<U32>(requiredExtensions.size());
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  if (g_EnableValidation) {
    
  }

  VkCall(vkCreateInstance(&createInfo, nullptr, &vkContext.Instance));

  return vkContext.Instance != VK_NULL_HANDLE;
}

const bool Renderer::CreateDebugMessenger() {
  Logger::Trace("Creating Vulkan debug messenger.");

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  FillDebugMessengerInfo(createInfo);

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      vkContext.Instance, "vkCreateDebugUtilsMessengerEXT");
  if (func) {
    VkCall(func(vkContext.Instance, &createInfo, nullptr, &vkContext.DebugMessenger));
  } else {
    Logger::Fatal("Failed to locate function vkCreateDebugUtilsMessengerEXT!");
    return false;
  }

  return vkContext.DebugMessenger != VK_NULL_HANDLE;
}

void Renderer::DestroyDebugMessenger() {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      vkContext.Instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func) {
    func(vkContext.Instance, vkContext.DebugMessenger, nullptr);
  }
}

void Renderer::DestroyInstance() { vkDestroyInstance(vkContext.Instance, nullptr); }

const bool Renderer::ValidateInstanceExtensions(const std::vector<const char*> extensions) {
  U32 availableExtensionCount = 0;
  VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr));
  std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
  VkCall(vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount,
                                                availableExtensions.data()));

  for (U32 i = 0; i < extensions.size(); i++) {
    bool found = false;
    for (U32 j = 0; j < availableExtensionCount; j++) {
      if (strcmp(extensions[i], availableExtensions[j].extensionName) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      Logger::Debug("ValidateInstanceExtensions failed to find required extension \"%s\"",
                    extensions[i]);
      return false;
    }
  }

  return true;
}
const bool Renderer::ValidateInstanceLayers(const std::vector<const char*> layers) {
  U32 availableLayerCount = 0;
  VkCall(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
  std::vector<VkLayerProperties> availableLayers(availableLayerCount);
  VkCall(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

  for (U32 i = 0; i < layers.size(); i++) {
    bool found = false;
    for (U32 j = 0; j < availableLayerCount; j++) {
      if (strcmp(layers[i], availableLayers[j].layerName) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      Logger::Debug("ValidateInstanceLayers failed to find required layer \"%s\"", layers[i]);
      return false;
    }
  }

  return true;
}

void Renderer::FillDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  createInfo.flags = 0;
  createInfo.pfnUserCallback = VulkanDebugCallback;
  createInfo.pNext = nullptr;
  createInfo.pUserData = nullptr;
}
}  // namespace Onyx