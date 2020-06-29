#include "Renderer.h"

#include <vulkan/vulkan.h>

#include "Application.h"
#include "Logger.h"

#define VkCall(result) \
  { ASSERT(result == VK_SUCCESS); }

namespace Onyx {
static struct VulkanContext { VkInstance Instance = VK_NULL_HANDLE; } vkContext;

static const std::vector<const char*> g_RequiredInstanceExtensions = {
    "VK_KHR_surface", "VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

const bool Renderer::Initialize() {
  Logger::Info("Initializing renderer...");

  ASSERT(CreateInstance());

  Logger::Info("Renderer finished initialization.");
  return true;
}

void Renderer::Shutdown() {
  Logger::Info("Renderer shutting down.");
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

  if (!ValidateInstanceExtensions(g_RequiredInstanceExtensions)) {
    Logger::Fatal("Could not load required Vulkan instance extensions!");
    return false;
  }
  Logger::Trace("Loading %d instance extensions:", g_RequiredInstanceExtensions.size());
  for (U32 i = 0; i < g_RequiredInstanceExtensions.size(); i++) {
    Logger::Trace(" - %s", g_RequiredInstanceExtensions[i]);
  }
  createInfo.enabledExtensionCount = static_cast<U32>(g_RequiredInstanceExtensions.size());
  createInfo.ppEnabledExtensionNames = g_RequiredInstanceExtensions.data();

  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;

  VkCall(vkCreateInstance(&createInfo, nullptr, &vkContext.Instance));

  return vkContext.Instance != VK_NULL_HANDLE;
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
}  // namespace Onyx