#include "VulkanRendererBackend.h"

#include <vector>

#include "Logger.h"
#include "Platform/VulkanPlatform.h"
#include "Renderer/Backend/Vulkan/VulkanDebugger.h"

namespace Onyx {
VulkanRendererBackend::VulkanRendererBackend(IApplication* application) {
  Logger::Trace("Creating Vulkan renderer backend...");
  _application = application;
  _debugger = new VulkanDebugger();
}

VulkanRendererBackend::~VulkanRendererBackend() { delete _debugger; }

const bool VulkanRendererBackend::Initialize(const bool enableValidation) {
  Logger::Trace("Initializing Vulkan renderer backend...");
  _validationEnabled = enableValidation;

  CreateInstance();

  if (_validationEnabled) {
    _debugger->Initialize(_instance, VulkanDebugger::Level::WARNING);
  }

  return false;
}

void VulkanRendererBackend::Shutdown() {
  if (_validationEnabled) {
    if (_debugger) {
      _debugger->Shutdown();
    }
  }

  if (_instance) {
    vkDestroyInstance(_instance, nullptr);
  }
}

const bool VulkanRendererBackend::PrepareFrame(const F32 deltaTime) {
  return false;
}

const bool VulkanRendererBackend::Frame(const F32 deltaTime) { return false; }

const bool VulkanRendererBackend::ValidationEnabled() const { return false; }

const bool VulkanRendererBackend::IsShutdown() const { return false; }

void VulkanRendererBackend::CreateInstance() {
  VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.apiVersion = VK_API_VERSION_1_2;
  appInfo.pApplicationName = "Onyx";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo instanceCreateInfo{
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  instanceCreateInfo.pApplicationInfo = &appInfo;

  std::vector<const char*> requiredExtensions;
  VulkanPlatform::GetRequiredExtensions(requiredExtensions);
  instanceCreateInfo.enabledExtensionCount =
      static_cast<uint32_t>(requiredExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

  if (_validationEnabled) {
    _requiredLayers.push_back("VK_LAYER_KHRONOS_validation");

    U32 availableLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    vkEnumerateInstanceLayerProperties(&availableLayerCount,
                                       availableLayers.data());

    bool success = true;
    for (U32 i = 0; i < (U32)_requiredLayers.size(); i++) {
      bool found = false;
      for (U32 j = 0; j < availableLayerCount; j++) {
        if (strcmp(_requiredLayers[i], availableLayers[j].layerName) == 0) {
          found = true;
          break;
        }
      }

      if (!found) {
        success = false;
        Logger::Fatal("Required validation layer missing: %s",
                      _requiredLayers[i]);
        break;
      }
    }

    instanceCreateInfo.enabledLayerCount =
        static_cast<uint32_t>(_requiredLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = _requiredLayers.data();
  } else {
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
  }

  vkCreateInstance(&instanceCreateInfo, nullptr, &_instance);
}
}  // namespace Onyx