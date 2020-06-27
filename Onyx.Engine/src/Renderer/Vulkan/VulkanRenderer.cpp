#include "pch.h"

#include "VulkanRenderer.h"

#include <set>
#include <vector>

#include "Platform/Application.h"
#include "Platform/VulkanPlatform.h"
#include "Renderer/Vulkan/VulkanCommandBuffer.h"
#include "Renderer/Vulkan/VulkanCommandPool.h"
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

VulkanRenderer::VulkanRenderer(const bool enableValidation)
    : IRenderer(enableValidation), _validationEnabled(enableValidation) {
  Logger::Info("Initializing Vulkan renderer...");

  if (!CreateInstance()) {
    Logger::Fatal("Failed to create Vulkan instance!");
  }

  if (_validationEnabled) {
    CreateDebugger(DebuggerLevel::Warning);
    Logger::Debug("Validation enabled. Debugger initialized.");
  }

  m_Surface = new VulkanSurface(this);

  m_Device = new VulkanDevice(m_Instance, _validationEnabled, m_RequiredLayers, m_Surface);

  Extent2D windowExtent = Application::GetWindowExtent();
  m_Swapchain = new VulkanSwapchain(m_Device, m_Surface, windowExtent);

  m_RenderPass = new VulkanRenderPass(m_Device, m_Swapchain);
  m_Shader = new VulkanShader(m_Device, "Basic", m_RenderPass, true, true, false, false);

  U32 imageCount = m_Swapchain->GetImageCount();
  m_CommandBuffers.resize(imageCount);
  m_ImageAvailableSemaphores.resize(imageCount);
  m_RenderFinishedSemaphores.resize(imageCount);
  m_InFlightFences.resize(imageCount);
  m_ImagesInFlight.resize(imageCount);
  for (U32 i = 0; i < imageCount; i++) {
    m_CommandBuffers[i] = m_Device->GetGraphicsCommandPool()->AllocateCommandBuffer(true);
    m_ImageAvailableSemaphores[i] = new VulkanSemaphore(m_Device);
    m_RenderFinishedSemaphores[i] = new VulkanSemaphore(m_Device);
    m_InFlightFences[i] = new VulkanFence(m_Device, true);
  }

  m_Vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
  m_Indices = {0, 1, 2, 2, 3, 0};

  size_t vertexDataSize = m_Vertices.size() * sizeof(Vertex);
  size_t indexDataSize = m_Indices.size() * sizeof(U16);
  m_VertexBuffer.Create(*m_Device, vertexDataSize);
  m_IndexBuffer.Create(*m_Device, indexDataSize);

  m_VertexBuffer.Upload(*m_Device, m_Vertices.data(), vertexDataSize);
  m_IndexBuffer.Upload(*m_Device, m_Indices.data(), indexDataSize);
}

VulkanRenderer::~VulkanRenderer() {
  if (m_Device) {
    m_Device->WaitIdle();
  }

  for (VulkanFence* fence : m_InFlightFences) {
    delete fence;
  }
  for (VulkanSemaphore* semaphore : m_ImageAvailableSemaphores) {
    delete semaphore;
  }
  for (VulkanSemaphore* semaphore : m_RenderFinishedSemaphores) {
    delete semaphore;
  }
  for (VulkanCommandBuffer* buffer : m_CommandBuffers) {
    m_Device->GetGraphicsCommandPool()->FreeCommandBuffer(buffer);
  }
  delete m_Shader;
  delete m_RenderPass;
  delete m_Swapchain;
  delete m_Device;
  delete m_Surface;

  if (_validationEnabled) {
    DestroyDebugger();
  }

  DestroyInstance();
}

const bool VulkanRenderer::PrepareFrame() {
  m_InFlightFences[m_CurrentFrame]->Wait();

  m_Swapchain->AcquireNextImage(m_ImageAvailableSemaphores[m_CurrentFrame], &m_CurrentImageIndex);

  if (m_ImagesInFlight[m_CurrentImageIndex] != nullptr) {
    m_ImagesInFlight[m_CurrentImageIndex]->Wait();
  }
  m_ImagesInFlight[m_CurrentImageIndex] = m_InFlightFences[m_CurrentFrame];

  VulkanCommandBuffer* cmdBuf = m_CommandBuffers[m_CurrentImageIndex];
  cmdBuf->Reset();
  cmdBuf->Begin();
  cmdBuf->BeginRenderPass(m_RenderPass, m_RenderPass->GetFramebuffer(m_CurrentImageIndex));
  cmdBuf->BindPipeline(m_Shader->GetPipeline());
  VkDeviceSize offsets[1] = {0};
  cmdBuf->BindVertexBuffers(1, &m_VertexBuffer, offsets);
  cmdBuf->BindIndexBuffer(m_IndexBuffer);
  cmdBuf->DrawIndexed(static_cast<U32>(m_Indices.size()));
  cmdBuf->EndRenderPass();
  cmdBuf->End();

  return true;
}

const bool VulkanRenderer::Frame() {
  VulkanCommandBuffer* cmdBuf = m_CommandBuffers[m_CurrentImageIndex];

  m_InFlightFences[m_CurrentFrame]->Reset();

  m_Device->GetGraphicsQueue()->Submit(cmdBuf,
                                       {m_ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore()},
                                       {m_RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore()},
                                       m_InFlightFences[m_CurrentFrame]->GetFence());

  m_Swapchain->Present(m_Device->GetGraphicsQueue(), m_Device->GetPresentQueue(),
                       {m_RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore()},
                       m_CurrentImageIndex);

  m_CurrentFrame = (m_CurrentFrame + 1) % s_MaxFramesInFlight;

  return true;
}

const bool VulkanRenderer::CreateInstance() {
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
  VulkanPlatform::GetRequiredExtensions(m_RequiredExtensions);
  if (!VerifyInstanceExtensions(m_RequiredExtensions)) {
    Logger::Fatal("Vulkan Instance does not support required extensions!");
    return false;
  }
  instanceCreateInfo.enabledExtensionCount = static_cast<U32>(m_RequiredExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames = m_RequiredExtensions.data();

  // If we want validation layers, ensure we can use them.
  if (_validationEnabled) {
    m_RequiredLayers.push_back("VK_LAYER_KHRONOS_validation");
  }

  if (!VerifyInstanceLayers(m_RequiredLayers)) {
    Logger::Fatal("Vulkan Instance does not support required layers!");
    return false;
  }
  instanceCreateInfo.enabledLayerCount = static_cast<U32>(m_RequiredLayers.size());
  instanceCreateInfo.ppEnabledLayerNames = m_RequiredLayers.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (_validationEnabled) {
    FillDebuggerInfo(DebuggerLevel::Warning, debugCreateInfo);
    instanceCreateInfo.pNext = &debugCreateInfo;
  }

  VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance));

  return true;
}

const bool VulkanRenderer::VerifyInstanceExtensions(
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

const bool VulkanRenderer::VerifyInstanceLayers(const std::vector<const char*>& requiredLayers) {
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

const bool VulkanRenderer::CreateDebugger(DebuggerLevel level) {
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  FillDebuggerInfo(DebuggerLevel::Warning, debugCreateInfo);
  debugCreateInfo.pUserData = this;

  static PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance,
                                                                "vkCreateDebugUtilsMessengerEXT");
  ASSERT_MSG(func, "Could not locate vkCreateDebugUtilsMessengerEXT!");

  VK_CHECK(func(m_Instance, &debugCreateInfo, nullptr, &m_Debugger));

  return true;
}

void VulkanRenderer::DestroyDebugger() {
  static PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance,
                                                                 "vkDestroyDebugUtilsMessengerEXT");
  if (func) {  // Must not throw/assert during destruction
    func(m_Instance, m_Debugger, nullptr);
  }
}

void VulkanRenderer::DestroyInstance() { vkDestroyInstance(m_Instance, nullptr); }

void VulkanRenderer::FillDebuggerInfo(DebuggerLevel level,
                                      VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo) {
  U32 severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  if (level >= DebuggerLevel::Warning) {
    severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  }
  if (level >= DebuggerLevel::Info) {
    severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
  }
  if (level >= DebuggerLevel::Trace) {
    severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  }

  debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugCreateInfo.messageSeverity = severity;
  debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  debugCreateInfo.pfnUserCallback = VulkanDebuggerCallback;
}
}  // namespace Vulkan
}  // namespace Onyx