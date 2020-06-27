#include "pch.h"

#include "VulkanRendererBackend.h"

#include <set>
#include <vector>

#include "Platform/Application.h"
#include "Platform/VulkanPlatform.h"
#include "Renderer/Vulkan/VulkanCommandBuffer.h"
#include "Renderer/Vulkan/VulkanCommandPool.h"
#include "Renderer/Vulkan/VulkanDebugger.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanFence.h"
#include "Renderer/Vulkan/VulkanQueue.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanSemaphore.h"
#include "Renderer/Vulkan/VulkanShader.h"
#include "Renderer/Vulkan/VulkanSurface.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"
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
    Logger::Debug("Validation enabled. Debugger initialized.");
  }

  _surface = new VulkanSurface(this);

  _device = new VulkanDevice(_instance, _validationEnabled, _requiredLayers, _surface);

  Extent2D windowExtent = Application::GetWindowExtent();
  _swapchain = new VulkanSwapchain(_device, _surface, windowExtent);

  _renderPass = new VulkanRenderPass(_device, _swapchain);
  _shader = new VulkanShader(_device, "Basic", _renderPass, true, true, false, false);

  U32 imageCount = _swapchain->GetImageCount();
  _commandBuffers.resize(imageCount);
  _imageAvailableSemaphores.resize(imageCount);
  _renderFinishedSemaphores.resize(imageCount);
  _inFlightFences.resize(imageCount);
  _imagesInFlight.resize(imageCount);
  for (U32 i = 0; i < imageCount; i++) {
    _commandBuffers[i] = _device->GetGraphicsCommandPool()->AllocateCommandBuffer(true);
    _imageAvailableSemaphores[i] = new VulkanSemaphore(_device);
    _renderFinishedSemaphores[i] = new VulkanSemaphore(_device);
    _inFlightFences[i] = new VulkanFence(_device, true);
  }
}

VulkanRendererBackend::~VulkanRendererBackend() {
  if (_device) {
    _device->WaitIdle();
  }

  for (VulkanFence* fence : _inFlightFences) {
    delete fence;
  }
  for (VulkanSemaphore* semaphore : _imageAvailableSemaphores) {
    delete semaphore;
  }
  for (VulkanSemaphore* semaphore : _renderFinishedSemaphores) {
    delete semaphore;
  }
  for (VulkanCommandBuffer* buffer : _commandBuffers) {
    _device->GetGraphicsCommandPool()->FreeCommandBuffer(buffer);
  }
  delete _shader;
  delete _renderPass;
  delete _swapchain;
  delete _device;
  delete _surface;

  if (_validationEnabled) {
    delete _debugger;
  }

  DestroyInstance();
}

const bool VulkanRendererBackend::PrepareFrame() {
  _inFlightFences[_currentFrame]->Wait();

  _swapchain->AcquireNextImage(_imageAvailableSemaphores[_currentFrame], &_currentImageIndex);
  
  if (_imagesInFlight[_currentImageIndex] != nullptr) {
    _imagesInFlight[_currentImageIndex]->Wait();
  }
  _imagesInFlight[_currentImageIndex] = _inFlightFences[_currentFrame];
  
  VulkanCommandBuffer* cmdBuf = _commandBuffers[_currentImageIndex];
  cmdBuf->Reset();
  cmdBuf->Begin();
  cmdBuf->BeginRenderPass(_renderPass, _renderPass->GetFramebuffer(_currentImageIndex));
  cmdBuf->BindPipeline(_shader->GetPipeline());
  cmdBuf->Draw(6, 1, 0, 1);
  cmdBuf->EndRenderPass();
  cmdBuf->End();

  return true;
}

const bool VulkanRendererBackend::Frame() {
  VulkanCommandBuffer* cmdBuf = _commandBuffers[_currentImageIndex];

  _inFlightFences[_currentFrame]->Reset();
  
  _device->GetGraphicsQueue()->Submit(
      cmdBuf, {_imageAvailableSemaphores[_currentFrame]->GetSemaphore()},
      {_renderFinishedSemaphores[_currentFrame]->GetSemaphore()}, _inFlightFences[_currentFrame]->GetFence());
  
  _swapchain->Present(_device->GetGraphicsQueue(), _device->GetPresentQueue(),
                      {_renderFinishedSemaphores[_currentFrame]->GetSemaphore()},
                      _currentImageIndex);

  _currentFrame = (_currentFrame + 1) % _maxFramesInFlight;

  return true;
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