#include "pch.h"

#include "VulkanRendererBackend.h"

#include <set>
#include <vector>

#include "Platform/IApplication.h"
#include "Platform/IWindow.h"
#include "Platform/VulkanPlatform.h"
#include "Renderer/Vulkan/VulkanDebugger.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanSurface.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanRendererBackend::VulkanRendererBackend(Platform::IApplication* application,
                                             const bool enableValidation)
    : IRendererBackend(application, enableValidation),
      _application(application),
      _validationEnabled(enableValidation) {
  Logger::Info("Initializing Vulkan renderer...");

  if (!CreateInstance()) {
    Logger::Fatal("Failed to create Vulkan instance!");
  }

  if (_validationEnabled) {
    _debugger = new VulkanDebugger(_instance, VulkanDebugger::Level::WARNING);
  }

  Platform::IWindow* applicationWindow = _application->GetApplicationWindow();
  _surface = new VulkanSurface(this, applicationWindow->GetHandle());

  _device = new VulkanDevice(_instance, _validationEnabled, _requiredLayers, _surface);
}

VulkanRendererBackend::~VulkanRendererBackend() {
  delete _device;
  delete _surface;

  if (_validationEnabled) {
    delete _debugger;
  }

  DestroyInstance();
}

const bool VulkanRendererBackend::CreateInstance() {
  // Check our Vulkan version and log it.
  // TODO: Ensure Vulkan version is high enough to support our app.
  U32 vulkanVersion;
  VK_CHECK(vkEnumerateInstanceVersion(&vulkanVersion));
  Logger::Debug("Vulkan Version %d.%d.%d", VK_VERSION_MAJOR(vulkanVersion),
                VK_VERSION_MINOR(vulkanVersion), VK_VERSION_PATCH(vulkanVersion));

  // Fill out basic information about our application.
  // TODO: Fill in name and versioning in a non-hardcoded way.
  VkApplicationInfo applicationInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
  applicationInfo.pApplicationName = "Onyx Engine";
  applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.pEngineName = "Onyx";
  applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.apiVersion = VK_API_VERSION_1_1;

  // Begin filling in our instance creation info.
  VkInstanceCreateInfo instanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  instanceCreateInfo.flags = 0;
  instanceCreateInfo.pApplicationInfo = &applicationInfo;

  // Determine what instance extensions we require, and make sure they're available.
  VulkanPlatform::GetRequiredExtensions(_requiredExtensions);
  if (!VerifyInstanceExtensions(_requiredExtensions)) {
    Logger::Fatal("Vulkan Instance does not support required extensions!");
    return false;
  }
  instanceCreateInfo.enabledExtensionCount = static_cast<U32>(_requiredExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames = _requiredExtensions.data();

  // If we want validation layers, ensure we can use them.
  if (_validationEnabled) {
    _requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
  }

  if (!VerifyInstanceLayers(_requiredLayers)) {
    Logger::Fatal("Vulkan Instance does not support required layers!");
    return false;
  }
  instanceCreateInfo.enabledLayerCount = static_cast<U32>(_requiredLayers.size());
  instanceCreateInfo.ppEnabledLayerNames = _requiredLayers.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  if (_validationEnabled) {
    debugCreateInfo = VulkanDebugger::GetCreateInfo(VulkanDebugger::Level::WARNING);
    instanceCreateInfo.pNext = &debugCreateInfo;
  }

  VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &_instance));

  return true;
}

const bool VulkanRendererBackend::VerifyInstanceExtensions(
    const std::vector<const char*>& requiredExtensions) {
  U32 availableExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount,
                                         availableExtensions.data());

  bool found;
  for (const char* extension : requiredExtensions) {
    found = false;
    for (VkExtensionProperties& properties : availableExtensions) {
      if (strcmp(extension, properties.extensionName) == 0) {
        found = true;
        break;
      }
    }

    if (!found) {
      return false;
    }
  }

  return true;
}

const bool VulkanRendererBackend::VerifyInstanceLayers(
    const std::vector<const char*>& requiredLayers) {
  U32 availableLayerCount = 0;
  vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(availableLayerCount);
  vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

  bool found;
  for (const char* extension : requiredLayers) {
    found = false;
    for (VkLayerProperties& properties : availableLayers) {
      if (strcmp(extension, properties.layerName) == 0) {
        found = true;
        break;
      }
    }

    if (!found) {
      return false;
    }
  }

  return true;
}

void VulkanRendererBackend::DestroyInstance() { vkDestroyInstance(_instance, nullptr); }
}  // namespace Vulkan
}  // namespace Onyx