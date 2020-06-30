#include "Renderer.h"

#include <Windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include <algorithm>

#include "Application.h"
#include "Logger.h"
#include "Utilities.h"

#define VkCall(result) \
  { ASSERT(result == VK_SUCCESS); }

namespace Onyx {
static VulkanContext vkContext{};

static const std::vector<const char*> g_RequiredInstanceExtensions = {"VK_KHR_surface",
                                                                      "VK_KHR_win32_surface"};
static const std::vector<const char*> g_RequiredInstanceValidationExtensions = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
static const std::vector<const char*> g_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
static const std::vector<const char*> g_RequiredDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};
static const U32 g_MaxFramesInFlight = 2;

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
  ASSERT(CreateSurface());
  ASSERT(SelectPhysicalDevice());
  Logger::Debug("Selected device: %s", vkContext.PhysicalDeviceInfo.Properties.deviceName);
  ASSERT(CreateDevice());
  ASSERT(GetDeviceQueues());
  ASSERT(CreateCommandPools());
  ASSERT(CreateSwapchainObjects());

  Logger::Info("Renderer finished initialization.");
  return true;
}

void Renderer::Shutdown() {
  Logger::Info("Renderer shutting down.");
  vkDeviceWaitIdle(vkContext.Device);
  DestroySwapchainObjects();
  DestroyCommandPools();
  DestroyDevice();
  DestroySurface();
  DestroyDebugMessenger();
  DestroyInstance();
}

const bool Renderer::Frame() {
  U32 imageIndex;

  vkWaitForFences(vkContext.Device, 1, &vkContext.InFlightFences[vkContext.CurrentFrame], VK_TRUE,
                  U64_MAX);

  VkResult acquireImageResult = vkAcquireNextImageKHR(
      vkContext.Device, vkContext.Swapchain, U64_MAX,
      vkContext.ImageAvailableSemaphores[vkContext.CurrentFrame], VK_NULL_HANDLE, &imageIndex);
  if (acquireImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreateSwapchain();
    return true;
  } else if (acquireImageResult != VK_SUCCESS && acquireImageResult != VK_SUBOPTIMAL_KHR) {
    return false;
  }

  if (vkContext.ImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(vkContext.Device, 1, &vkContext.ImagesInFlight[imageIndex], VK_TRUE, U64_MAX);
  }
  vkContext.ImagesInFlight[imageIndex] = vkContext.InFlightFences[vkContext.CurrentFrame];

  VkCommandBuffer& cmdBuf = vkContext.GraphicsCommandBuffers[imageIndex];
  BeginCommandBuffer(cmdBuf);
  BeginRenderPass(cmdBuf, vkContext.SwapchainFramebuffers[imageIndex]);
  BindGraphicsPipeline(cmdBuf);
  Draw(cmdBuf, 3, 1, 0, 0);
  EndRenderPass(cmdBuf);
  EndCommandBuffer(cmdBuf);

  vkResetFences(vkContext.Device, 1, &vkContext.InFlightFences[vkContext.CurrentFrame]);

  VkSemaphore waitSemaphores[] = {vkContext.ImageAvailableSemaphores[vkContext.CurrentFrame]};
  VkSemaphore signalSemaphores[] = {vkContext.RenderFinishedSemaphores[vkContext.CurrentFrame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &vkContext.GraphicsCommandBuffers[imageIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  VkCall(vkQueueSubmit(vkContext.GraphicsQueue, 1, &submitInfo,
                       vkContext.InFlightFences[vkContext.CurrentFrame]));

  VkSwapchainKHR swapchains[] = {vkContext.Swapchain};
  VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  VkResult presentResult = vkQueuePresentKHR(vkContext.PresentationQueue, &presentInfo);
  if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
    RecreateSwapchain();
  } else if (presentResult != VK_SUCCESS) {
    return false;
  }

  vkContext.CurrentFrame = (vkContext.CurrentFrame + 1) % g_MaxFramesInFlight;

  return true;
}

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

const bool Renderer::CreateSurface() {
  Logger::Trace("Creating Vulkan surface.");

  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
  surfaceCreateInfo.hinstance = static_cast<HINSTANCE>(Application::GetInstance());
  surfaceCreateInfo.hwnd = static_cast<HWND>(Application::GetWindow());
  VkCall(
      vkCreateWin32SurfaceKHR(vkContext.Instance, &surfaceCreateInfo, nullptr, &vkContext.Surface));

  return vkContext.Surface != VK_NULL_HANDLE;
}

const bool Renderer::CreateDevice() {
  Logger::Trace("Creating Vulkan logical device.");

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(vkContext.PhysicalDeviceInfo.Queues.Count);
  const float queuePriority = 1.0f;
  for (U32 i = 0; i < vkContext.PhysicalDeviceInfo.Queues.Count; i++) {
    queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[i].queueFamilyIndex = vkContext.PhysicalDeviceInfo.Queues.Queues[i].Index;
    queueCreateInfos[i].queueCount = 1;
    queueCreateInfos[i].pQueuePriorities = &queuePriority;
  }

  VkPhysicalDeviceFeatures requestedFeatures{};

  VkDeviceCreateInfo deviceCreateInfo{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  deviceCreateInfo.queueCreateInfoCount = static_cast<U32>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.pEnabledFeatures = &requestedFeatures;

  if (g_EnableValidation) {
    Logger::Trace("Loading device validation layers.");
    deviceCreateInfo.enabledLayerCount = static_cast<U32>(g_ValidationLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = g_ValidationLayers.data();
  }

  Logger::Trace("Loading %d device extensions:", g_RequiredDeviceExtensions.size());
  for (U32 i = 0; i < g_RequiredDeviceExtensions.size(); i++) {
    Logger::Trace(" - %s", g_RequiredDeviceExtensions[i]);
  }
  deviceCreateInfo.enabledExtensionCount = static_cast<U32>(g_RequiredDeviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = g_RequiredDeviceExtensions.data();

  VkCall(vkCreateDevice(vkContext.PhysicalDevice, &deviceCreateInfo, nullptr, &vkContext.Device));

  return vkContext.Device != VK_NULL_HANDLE;
}

const bool Renderer::CreateSwapchain() {
  Logger::Trace("Creating Vulkan swapchain.");

  ASSERT(GetSwapchainSurfaceFormat());
  ASSERT(GetSwapchainPresentMode());
  ASSERT(GetSwapchainExtent());
  vkContext.SwapchainImageCount =
      std::min(vkContext.PhysicalDeviceInfo.SwapchainSupport.Capabilities.minImageCount + 1,
               vkContext.PhysicalDeviceInfo.SwapchainSupport.Capabilities.maxImageCount);
  Logger::Trace("Swapchain will have %d images.", vkContext.SwapchainImageCount);

  VkSwapchainCreateInfoKHR swapchainCreateInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  swapchainCreateInfo.surface = vkContext.Surface;
  swapchainCreateInfo.minImageCount = vkContext.SwapchainImageCount;
  swapchainCreateInfo.imageFormat = vkContext.SwapchainSurfaceFormat.format;
  swapchainCreateInfo.imageColorSpace = vkContext.SwapchainSurfaceFormat.colorSpace;
  swapchainCreateInfo.imageExtent = vkContext.SwapchainExtent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCreateInfo.preTransform =
      vkContext.PhysicalDeviceInfo.SwapchainSupport.Capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = vkContext.SwapchainPresentMode;
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  U32 queueFamilyIndices[] = {
      static_cast<U32>(vkContext.PhysicalDeviceInfo.Queues.GraphicsIndex),
      static_cast<U32>(vkContext.PhysicalDeviceInfo.Queues.PresentationIndex)};
  if (vkContext.PhysicalDeviceInfo.Queues.GraphicsIndex !=
      vkContext.PhysicalDeviceInfo.Queues.PresentationIndex) {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  VkCall(
      vkCreateSwapchainKHR(vkContext.Device, &swapchainCreateInfo, nullptr, &vkContext.Swapchain));

  return vkContext.Swapchain != VK_NULL_HANDLE;
}

const bool Renderer::CreateSwapchainImages() {
  Logger::Trace("Creating swapchain images and image views.");
  VkCall(vkGetSwapchainImagesKHR(vkContext.Device, vkContext.Swapchain,
                                 &vkContext.SwapchainImageCount, nullptr));
  vkContext.SwapchainImages.resize(vkContext.SwapchainImageCount);
  vkContext.SwapchainImageViews.resize(vkContext.SwapchainImageCount);
  VkCall(vkGetSwapchainImagesKHR(vkContext.Device, vkContext.Swapchain,
                                 &vkContext.SwapchainImageCount, vkContext.SwapchainImages.data()));

  for (U32 i = 0; i < vkContext.SwapchainImageCount; i++) {
    VkImageViewCreateInfo imageViewCreateInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    imageViewCreateInfo.image = vkContext.SwapchainImages[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = vkContext.SwapchainSurfaceFormat.format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    VkCall(vkCreateImageView(vkContext.Device, &imageViewCreateInfo, nullptr,
                             &vkContext.SwapchainImageViews[i]));
    if (vkContext.SwapchainImageViews[i] == VK_NULL_HANDLE) {
      return false;
    }
  }

  return true;
}

const bool Renderer::CreateRenderPass() {
  Logger::Trace("Creating render pass.");

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = vkContext.SwapchainSurfaceFormat.format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassCreateInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  renderPassCreateInfo.attachmentCount = 1;
  renderPassCreateInfo.pAttachments = &colorAttachment;
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpass;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &dependency;

  VkCall(
      vkCreateRenderPass(vkContext.Device, &renderPassCreateInfo, nullptr, &vkContext.RenderPass));

  return vkContext.RenderPass != VK_NULL_HANDLE;
}

const bool Renderer::CreateGraphicsPipeline() {
  Logger::Trace("Creating graphics pipeline.");

  std::vector<char> vertexShaderSource = Utilities::ReadBinaryFile("assets/shaders/Basic.vert.spv");
  std::vector<char> fragmentShaderSource =
      Utilities::ReadBinaryFile("assets/shaders/Basic.frag.spv");

  VkShaderModule vertexShaderModule = CreateShaderModule(vertexShaderSource);
  VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentShaderSource);

  if (vertexShaderModule == VK_NULL_HANDLE || fragmentShaderModule == VK_NULL_HANDLE) {
    return false;
  }

  VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageCreateInfo.module = vertexShaderModule;
  vertexShaderStageCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageCreateInfo.module = fragmentShaderModule;
  fragmentShaderStageCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vertexShaderStageCreateInfo,
                                                              fragmentShaderStageCreateInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<F32>(vkContext.SwapchainExtent.width);
  viewport.height = static_cast<F32>(vkContext.SwapchainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = vkContext.SwapchainExtent;

  VkPipelineViewportStateCreateInfo viewportState{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  VkPipelineMultisampleStateCreateInfo multisampling{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamicStates;

  VkPipelineDynamicStateCreateInfo dynamicState{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  dynamicState.dynamicStateCount = static_cast<U32>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipelineLayoutCreateInfo.setLayoutCount = 0;
  pipelineLayoutCreateInfo.pSetLayouts = nullptr;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  VkCall(vkCreatePipelineLayout(vkContext.Device, &pipelineLayoutCreateInfo, nullptr,
                                &vkContext.PipelineLayout));
  if (vkContext.PipelineLayout == VK_NULL_HANDLE) {
    return false;
  }

  VkGraphicsPipelineCreateInfo pipelineCreateInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
  pipelineCreateInfo.stageCount = 2;
  pipelineCreateInfo.pStages = shaderStageCreateInfos;
  pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pRasterizationState = &rasterizer;
  pipelineCreateInfo.pMultisampleState = &multisampling;
  pipelineCreateInfo.pDepthStencilState = nullptr;
  pipelineCreateInfo.pColorBlendState = &colorBlending;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.layout = vkContext.PipelineLayout;
  pipelineCreateInfo.renderPass = vkContext.RenderPass;
  pipelineCreateInfo.subpass = 0;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineCreateInfo.basePipelineIndex = -1;

  VkCall(vkCreateGraphicsPipelines(vkContext.Device, VK_NULL_HANDLE, 1, &pipelineCreateInfo,
                                   nullptr, &vkContext.GraphicsPipeline));
  if (vkContext.GraphicsPipeline == VK_NULL_HANDLE) {
    return false;
  }

  DestroyShaderModule(fragmentShaderModule);
  DestroyShaderModule(vertexShaderModule);

  return true;
}

VkShaderModule Renderer::CreateShaderModule(const std::vector<char>& source) {
  VkShaderModuleCreateInfo shaderCreateInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  shaderCreateInfo.codeSize = static_cast<U32>(source.size());
  shaderCreateInfo.pCode = reinterpret_cast<const U32*>(source.data());

  VkShaderModule module;
  VkCall(vkCreateShaderModule(vkContext.Device, &shaderCreateInfo, nullptr, &module));
  return module;
}

const bool Renderer::CreateFramebuffers() {
  Logger::Trace("Creating framebuffers.");

  vkContext.SwapchainFramebuffers.resize(vkContext.SwapchainImageCount, VK_NULL_HANDLE);

  for (U32 i = 0; i < vkContext.SwapchainImageCount; i++) {
    VkImageView attachments[] = {vkContext.SwapchainImageViews[i]};

    VkFramebufferCreateInfo framebufferCreateInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferCreateInfo.renderPass = vkContext.RenderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = vkContext.SwapchainExtent.width;
    framebufferCreateInfo.height = vkContext.SwapchainExtent.height;
    framebufferCreateInfo.layers = 1;

    VkCall(vkCreateFramebuffer(vkContext.Device, &framebufferCreateInfo, nullptr,
                               &vkContext.SwapchainFramebuffers[i]));
    if (vkContext.SwapchainFramebuffers[i] == VK_NULL_HANDLE) {
      return false;
    }
  }

  return true;
}

const bool Renderer::CreateCommandPools() {
  Logger::Trace("Creating command pools.");

  VkCommandPoolCreateInfo graphicsPoolCreateInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  graphicsPoolCreateInfo.queueFamilyIndex = vkContext.PhysicalDeviceInfo.Queues.GraphicsIndex;
  graphicsPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkCall(vkCreateCommandPool(vkContext.Device, &graphicsPoolCreateInfo, nullptr,
                             &vkContext.GraphicsCommandPool));

  return vkContext.GraphicsCommandPool != VK_NULL_HANDLE;
}

const bool Renderer::AllocateGraphicsCommandBuffers() {
  vkContext.GraphicsCommandBuffers.resize(vkContext.SwapchainImageCount, VK_NULL_HANDLE);

  VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  allocateInfo.commandPool = vkContext.GraphicsCommandPool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = static_cast<U32>(vkContext.GraphicsCommandBuffers.size());

  VkCall(vkAllocateCommandBuffers(vkContext.Device, &allocateInfo,
                                  vkContext.GraphicsCommandBuffers.data()));

  return true;
}

const bool Renderer::CreateSyncObjects() {
  Logger::Trace("Creating sync objects.");

  vkContext.ImagesInFlight.clear();
  vkContext.ImageAvailableSemaphores.resize(g_MaxFramesInFlight, VK_NULL_HANDLE);
  vkContext.RenderFinishedSemaphores.resize(g_MaxFramesInFlight, VK_NULL_HANDLE);
  vkContext.InFlightFences.resize(g_MaxFramesInFlight, VK_NULL_HANDLE);
  vkContext.ImagesInFlight.resize(vkContext.SwapchainImageCount, VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VkFenceCreateInfo fenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (U32 i = 0; i < g_MaxFramesInFlight; i++) {
    VkCall(vkCreateSemaphore(vkContext.Device, &semaphoreCreateInfo, nullptr,
                             &vkContext.ImageAvailableSemaphores[i]));
    VkCall(vkCreateSemaphore(vkContext.Device, &semaphoreCreateInfo, nullptr,
                             &vkContext.RenderFinishedSemaphores[i]));
    VkCall(
        vkCreateFence(vkContext.Device, &fenceCreateInfo, nullptr, &vkContext.InFlightFences[i]));
    if (vkContext.ImageAvailableSemaphores[i] == VK_NULL_HANDLE ||
        vkContext.RenderFinishedSemaphores[i] == VK_NULL_HANDLE ||
        vkContext.InFlightFences[i] == VK_NULL_HANDLE) {
      return false;
    }
  }

  return true;
}

const bool Renderer::CreateSwapchainObjects() {
  ASSERT(CreateSwapchain());
  ASSERT(CreateSwapchainImages());
  ASSERT(CreateRenderPass());
  ASSERT(CreateGraphicsPipeline());
  ASSERT(CreateFramebuffers());
  ASSERT(AllocateGraphicsCommandBuffers());
  ASSERT(CreateSyncObjects());
  return true;
}

const bool Renderer::RecreateSwapchain() {
  Logger::Debug("Swapchain out of date or suboptimal, recreating.");
  vkDeviceWaitIdle(vkContext.Device);

  if (vkContext.Swapchain) {
    DestroySwapchainObjects();
  }

  return CreateSwapchainObjects();
}

void Renderer::DestroySyncObjects() {
  for (U32 i = 0; i < g_MaxFramesInFlight; i++) {
    vkDestroyFence(vkContext.Device, vkContext.InFlightFences[i], nullptr);
    vkDestroySemaphore(vkContext.Device, vkContext.RenderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(vkContext.Device, vkContext.ImageAvailableSemaphores[i], nullptr);
  }
}

void Renderer::FreeGraphicsCommandBuffers() {
  vkFreeCommandBuffers(vkContext.Device, vkContext.GraphicsCommandPool,
                       static_cast<U32>(vkContext.GraphicsCommandBuffers.size()),
                       vkContext.GraphicsCommandBuffers.data());
}

void Renderer::DestroyCommandPools() {
  vkDestroyCommandPool(vkContext.Device, vkContext.GraphicsCommandPool, nullptr);
}

void Renderer::DestroyFramebuffers() {
  for (VkFramebuffer framebuffer : vkContext.SwapchainFramebuffers) {
    vkDestroyFramebuffer(vkContext.Device, framebuffer, nullptr);
  }
}

void Renderer::DestroyShaderModule(VkShaderModule module) {
  vkDestroyShaderModule(vkContext.Device, module, nullptr);
}

void Renderer::DestroyGraphicsPipeline() {
  vkDestroyPipeline(vkContext.Device, vkContext.GraphicsPipeline, nullptr);
  vkDestroyPipelineLayout(vkContext.Device, vkContext.PipelineLayout, nullptr);
}

void Renderer::DestroyRenderPass() {
  vkDestroyRenderPass(vkContext.Device, vkContext.RenderPass, nullptr);
}

void Renderer::DestroySwapchainImages() {
  for (VkImageView imageView : vkContext.SwapchainImageViews) {
    vkDestroyImageView(vkContext.Device, imageView, nullptr);
  }
}

void Renderer::DestroySwapchain() {
  vkDestroySwapchainKHR(vkContext.Device, vkContext.Swapchain, nullptr);
}

void Renderer::DestroySwapchainObjects() {
  DestroySyncObjects();
  DestroyFramebuffers();
  DestroyGraphicsPipeline();
  DestroyRenderPass();
  DestroySwapchainImages();
  DestroySwapchain();
}

void Renderer::DestroyDevice() { vkDestroyDevice(vkContext.Device, nullptr); }

void Renderer::DestroySurface() {
  vkDestroySurfaceKHR(vkContext.Instance, vkContext.Surface, nullptr);
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

const bool Renderer::SelectPhysicalDevice() {
  Logger::Trace("Enumerating Vulkan physical devices.");

  U32 deviceCount = 0;
  VkCall(vkEnumeratePhysicalDevices(vkContext.Instance, &deviceCount, nullptr));
  if (deviceCount == 0) {
    Logger::Fatal("No Vulkan devices could be found!");
    return false;
  }
  Logger::Trace("Found %d Vulkan devices.", deviceCount);
  std::vector<VkPhysicalDevice> devices(deviceCount);
  VkCall(vkEnumeratePhysicalDevices(vkContext.Instance, &deviceCount, devices.data()));

  vkContext.PhysicalDevices.resize(deviceCount);
  for (U32 i = 0; i < deviceCount; i++) {
    vkContext.PhysicalDevices[i].Device = devices[i];
    QueryPhysicalDeviceInfo(devices[i], vkContext.PhysicalDevices[i]);
    DumpPhysicalDeviceInfo(vkContext.PhysicalDevices[i]);
  }

  for (U32 i = 0; i < deviceCount; i++) {
    if (ValidatePhysicalDevice(vkContext.PhysicalDevices[i])) {
      vkContext.PhysicalDevice = vkContext.PhysicalDevices[i].Device;
      vkContext.PhysicalDeviceInfo = vkContext.PhysicalDevices[i];
    }
  }

  return vkContext.PhysicalDevice != VK_NULL_HANDLE;
}

void Renderer::QueryPhysicalDeviceInfo(VkPhysicalDevice device,
                                       VulkanPhysicalDeviceInfo& deviceInfo) {
  vkGetPhysicalDeviceFeatures(device, &deviceInfo.Features);
  vkGetPhysicalDeviceMemoryProperties(device, &deviceInfo.Memory);
  vkGetPhysicalDeviceProperties(device, &deviceInfo.Properties);
  QueryPhysicalDeviceQueues(device, deviceInfo.Queues);
  QueryPhysicalDeviceSwapchainSupport(device, deviceInfo.SwapchainSupport);
  QueryPhysicalDeviceExtensions(device, deviceInfo.Extensions);
}

void Renderer::QueryPhysicalDeviceQueues(VkPhysicalDevice device,
                                         VulkanPhysicalDeviceQueues& queueInfo) {
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueInfo.Count, nullptr);
  std::vector<VkQueueFamilyProperties> families(queueInfo.Count);
  queueInfo.Queues.resize(queueInfo.Count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueInfo.Count, families.data());

  for (U32 i = 0; i < queueInfo.Count; i++) {
    VulkanPhysicalDeviceQueue& queue = queueInfo.Queues[i];
    queue.Index = i;
    queue.Flags = families[i].queueFlags;
    queue.Count = families[i].queueCount;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkContext.Surface, &queue.PresentKHR);

    if (queueInfo.GraphicsIndex == -1 && queue.SupportsGraphics()) {
      queueInfo.GraphicsIndex = i;
    }
    if (queueInfo.PresentationIndex == -1 && queue.SupportsPresentation()) {
      queueInfo.PresentationIndex = i;
    }

    if (queue.SupportsCompute() &&
        (queueInfo.ComputeIndex == -1 || queueInfo.ComputeIndex == queueInfo.GraphicsIndex)) {
      queueInfo.ComputeIndex = i;
    }
    if (queue.SupportsTransfer() &&
        (queueInfo.TransferIndex == -1 || queueInfo.TransferIndex == queueInfo.GraphicsIndex)) {
      queueInfo.TransferIndex = i;
    }
  }

  if (queueInfo.TransferIndex == -1) {
    queueInfo.TransferIndex = queueInfo.GraphicsIndex;
  }
}

void Renderer::QueryPhysicalDeviceSwapchainSupport(VkPhysicalDevice device,
                                                   VulkanPhysicalDeviceSwapchainSupport& support) {
  VkCall(
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vkContext.Surface, &support.Capabilities));

  U32 formatCount = 0;
  VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkContext.Surface, &formatCount, nullptr));
  if (formatCount != 0) {
    support.Formats.resize(formatCount);
    VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkContext.Surface, &formatCount,
                                                support.Formats.data()));
  }

  U32 presentModeCount = 0;
  VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkContext.Surface, &presentModeCount,
                                                   nullptr));
  if (presentModeCount != 0) {
    support.PresentationModes.resize(presentModeCount);
    VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkContext.Surface, &presentModeCount,
                                                     support.PresentationModes.data()));
  }
}

void Renderer::QueryPhysicalDeviceExtensions(VkPhysicalDevice device,
                                             std::vector<VkExtensionProperties>& extensions) {
  U32 extensionCount = 0;
  VkCall(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));
  if (extensionCount != 0) {
    extensions.resize(extensionCount);
    VkCall(
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data()));
  }
}

const bool Renderer::ValidatePhysicalDevice(VulkanPhysicalDeviceInfo& deviceInfo) {
  if (deviceInfo.Queues.GraphicsIndex == -1 || deviceInfo.Queues.PresentationIndex == -1) {
    Logger::Trace("Rejecting device: Missing graphics or presentation queue.");
    return false;
  }

  for (U32 i = 0; i < g_RequiredDeviceExtensions.size(); i++) {
    bool found = false;
    for (U32 j = 0; j < deviceInfo.Extensions.size(); j++) {
      if (strcmp(g_RequiredDeviceExtensions[i], deviceInfo.Extensions[j].extensionName) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      Logger::Trace("Rejecting device: Failed to find required extension \"%s\"",
                    g_RequiredDeviceExtensions[i]);
      return false;
    }
  }

  if (deviceInfo.SwapchainSupport.Formats.size() == 0 ||
      deviceInfo.SwapchainSupport.PresentationModes.size() == 0) {
    Logger::Trace("Rejecting device: No available image formats or presentation modes.");
    return false;
  }

  return true;
}

void Renderer::DumpPhysicalDeviceInfo(const VulkanPhysicalDeviceInfo& info) {
  Logger::Trace("Vulkan Device \"%s\":", info.Properties.deviceName);

  // General device details
  Logger::Trace(" - Vulkan API: %d.%d.%d", VK_VERSION_MAJOR(info.Properties.apiVersion),
                VK_VERSION_MINOR(info.Properties.apiVersion),
                VK_VERSION_PATCH(info.Properties.apiVersion));
  switch (info.Properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      Logger::Trace(" - Device Type: Dedicated");
      break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      Logger::Trace(" - Device Type: Integrated");
      break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      Logger::Trace(" - Device Type: Virtual");
      break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      Logger::Trace(" - Device Type: CPU");
      break;
    default:
      Logger::Trace(" - Device Type: Unknown");
      break;
  }
  Logger::Trace(" - Max 2D Resolution: %d", info.Properties.limits.maxImageDimension2D);

  // Memory Details
  Logger::Trace(" - Memory:");
  Logger::Trace("   - Types (%d):", info.Memory.memoryTypeCount);
  // DL = Device Local
  // HV = Host Visible
  // HC = Host Coherent
  // HH = Host Cached
  // LA = Lazily Allocated
  // PT = Protected
  // DC = Device Coherent (AMD)
  // DU = Device Uncached (AMD)
  Logger::Trace("               / DL | HV | HC | HH | LA | PT | DC | DU \\");
  for (U32 memoryTypeIndex = 0; memoryTypeIndex < info.Memory.memoryTypeCount; memoryTypeIndex++) {
    const VkMemoryType& memType = info.Memory.memoryTypes[memoryTypeIndex];
    Logger::Trace("     - Heap %d: | %s | %s | %s | %s | %s | %s | %s | %s |", memType.heapIndex,
                  memType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? "DL" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ? "HV" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ? "HC" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT ? "HH" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT ? "LA" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT ? "PT" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD ? "DC" : "  ",
                  memType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD ? "DU" : "  ");
  }
  Logger::Trace("   - Heaps (%d):", info.Memory.memoryHeapCount);
  for (U32 memoryHeapIndex = 0; memoryHeapIndex < info.Memory.memoryHeapCount; memoryHeapIndex++) {
    const VkMemoryHeap& memHeap = info.Memory.memoryHeaps[memoryHeapIndex];
    // DL = Device Local
    // MI = Multi Instance
    // MI = Multi Instance (KHR)
    Logger::Trace("     - Heap %d: %.2f MiB { %s | %s | %s }", memoryHeapIndex,
                  ((F32)memHeap.size / 1024.0f / 1024.0f),
                  memHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ? "DL" : "  ",
                  memHeap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT ? "MI" : "  ",
                  memHeap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHR ? "MK" : "  ");
  }

  Logger::Trace(" - Queue Families (%d):", info.Queues.Count);
  // GFX = Graphics
  // CMP = Compute
  // TRA = Transfer
  // SPB = Sparse Binding
  // PRT = Protected
  // PST = Presentation (KHR)
  Logger::Trace("               / GFX  | CMP  | TRA  | SPB  | PRT  | PST  \\");
  const VulkanPhysicalDeviceQueues& queues = info.Queues;
  for (U32 queueIndex = 0; queueIndex < info.Queues.Count; queueIndex++) {
    const VulkanPhysicalDeviceQueue& queue = info.Queues.Queues[queueIndex];
    // Asterisk indicates the Queue Family has been selected for that particular queue operation.
    Logger::Trace(
        " - Family %d: { %s%c | %s%c | %s%c | %s%c | %s%c | %s%c } (%d Queues)", queueIndex,
        queue.SupportsGraphics() ? "GFX" : "   ", queues.GraphicsIndex == queue.Index ? '*' : ' ',
        queue.SupportsCompute() ? "CMP" : "   ", queues.ComputeIndex == queue.Index ? '*' : ' ',
        queue.SupportsTransfer() ? "TRA" : "   ", queues.TransferIndex == queue.Index ? '*' : ' ',
        queue.SupportsSparseBinding() ? "SPB" : "   ", ' ',
        queue.SupportsProtected() ? "PRT" : "   ", ' ',
        queue.SupportsPresentation() ? "PST" : "   ",
        queues.PresentationIndex == queue.Index ? '*' : ' ', queue.Count);
  }

  // Swapchain details
  Logger::Trace("-- Swapchain:");
  Logger::Trace("---- Image Count: %d Min / %d Max",
                info.SwapchainSupport.Capabilities.minImageCount,
                info.SwapchainSupport.Capabilities.maxImageCount);
  Logger::Trace("---- Image Size: %dx%d Min / %dx%d Max",
                info.SwapchainSupport.Capabilities.minImageExtent.width,
                info.SwapchainSupport.Capabilities.minImageExtent.height,
                info.SwapchainSupport.Capabilities.maxImageExtent.width,
                info.SwapchainSupport.Capabilities.maxImageExtent.height);
  Logger::Trace("---- Image Formats: %d", info.SwapchainSupport.Formats.size());
  Logger::Trace("---- Present Modes: %d", info.SwapchainSupport.PresentationModes.size());

  // Extensions
  Logger::Trace("-- Extensions (%d):", info.Extensions.size());
  for (const VkExtensionProperties& ext : info.Extensions) {
    Logger::Trace("---- %s", ext.extensionName);
  }
}

const bool Renderer::GetDeviceQueues() {
  Logger::Trace("Retrieving device command queues.");

  vkGetDeviceQueue(vkContext.Device, vkContext.PhysicalDeviceInfo.Queues.GraphicsIndex, 0,
                   &vkContext.GraphicsQueue);
  vkGetDeviceQueue(vkContext.Device, vkContext.PhysicalDeviceInfo.Queues.PresentationIndex, 0,
                   &vkContext.PresentationQueue);
  vkGetDeviceQueue(vkContext.Device, vkContext.PhysicalDeviceInfo.Queues.TransferIndex, 0,
                   &vkContext.TransferQueue);

  return vkContext.GraphicsQueue != VK_NULL_HANDLE &&
         vkContext.PresentationQueue != VK_NULL_HANDLE && vkContext.TransferQueue != VK_NULL_HANDLE;
}
const bool Renderer::GetSwapchainSurfaceFormat() {
  for (const auto& availableFormat : vkContext.PhysicalDeviceInfo.SwapchainSupport.Formats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      vkContext.SwapchainSurfaceFormat = availableFormat;
      return true;
    }
  }

  vkContext.SwapchainSurfaceFormat = vkContext.PhysicalDeviceInfo.SwapchainSupport.Formats[0];
  return true;
}

const bool Renderer::GetSwapchainPresentMode() {
  for (const auto& availablePresentMode :
       vkContext.PhysicalDeviceInfo.SwapchainSupport.PresentationModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      vkContext.SwapchainPresentMode = availablePresentMode;
      return true;
    }
  }

  vkContext.SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  return true;
}

const bool Renderer::GetSwapchainExtent() {
  QueryPhysicalDeviceSwapchainSupport(vkContext.PhysicalDevice,
                                      vkContext.PhysicalDeviceInfo.SwapchainSupport);
  const VkSurfaceCapabilitiesKHR& capabilities =
      vkContext.PhysicalDeviceInfo.SwapchainSupport.Capabilities;
  if (capabilities.currentExtent.width != U32_MAX) {
    vkContext.SwapchainExtent = capabilities.currentExtent;
    return true;
  }

  Extent2D appExtent = Application::GetWindowExtent();
  vkContext.SwapchainExtent.width =
      std::max(capabilities.minImageExtent.width,
               std::min(capabilities.maxImageExtent.width, appExtent.Width));
  vkContext.SwapchainExtent.height =
      std::max(capabilities.minImageExtent.height,
               std::min(capabilities.maxImageExtent.height, appExtent.Height));

  return true;
}

void Renderer::BeginCommandBuffer(VkCommandBuffer buffer) {
  VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  VkCall(vkBeginCommandBuffer(buffer, &beginInfo));
}

void Renderer::BeginRenderPass(VkCommandBuffer buffer, VkFramebuffer framebuffer) {
  VkClearValue clearColor = {0.0f, 0.0f, 1.0f, 1.0f};
  VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  renderPassBeginInfo.renderPass = vkContext.RenderPass;
  renderPassBeginInfo.framebuffer = framebuffer;
  renderPassBeginInfo.renderArea.offset = {0, 0};
  renderPassBeginInfo.renderArea.extent = vkContext.SwapchainExtent;
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearColor;
  vkCmdBeginRenderPass(buffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::BindGraphicsPipeline(VkCommandBuffer buffer) {
  vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkContext.GraphicsPipeline);
}

void Renderer::Draw(VkCommandBuffer buffer, U32 vertexCount, U32 instanceCount, U32 firstVertex,
                    U32 firstInstance) {
  vkCmdDraw(buffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void Renderer::EndRenderPass(VkCommandBuffer buffer) { vkCmdEndRenderPass(buffer); }

void Renderer::EndCommandBuffer(VkCommandBuffer buffer) { VkCall(vkEndCommandBuffer(buffer)); }
}  // namespace Onyx