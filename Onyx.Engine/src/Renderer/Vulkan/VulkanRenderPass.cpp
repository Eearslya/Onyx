#include "pch.h"

#include "VulkanRenderPass.h"

#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, VulkanSwapchain* swapchain)
    : _device(device), _swapchain(swapchain) {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = _swapchain->GetImageFormat().format;
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

  VkRenderPassCreateInfo renderPassInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  VK_CHECK(vkCreateRenderPass(_device->GetLogicalDevice(), &renderPassInfo, nullptr, &_renderPass));
  Logger::Debug("Initialized Vulkan render pass.");
  CreateFramebuffers();
}

VulkanRenderPass::~VulkanRenderPass() {
  DestroyFramebuffers();
  vkDestroyRenderPass(_device->GetLogicalDevice(), _renderPass, nullptr);
}

VkExtent2D VulkanRenderPass::GetExtent() { return _swapchain->GetExtent(); }

void VulkanRenderPass::CreateFramebuffers() {
  U32 bufferCount = _swapchain->GetImageCount();
  _framebuffers.resize(bufferCount);

  for (U32 i = 0; i < bufferCount; i++) {
    VkImageView attachments[] = {_swapchain->GetImageView(i)};
    const VkExtent2D& extent = GetExtent();

    VkFramebufferCreateInfo framebufferInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = _renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(_device->GetLogicalDevice(), &framebufferInfo, nullptr,
                                 &_framebuffers[i]));
  }

  Logger::Debug("Created %d Framebuffers for Render Pass.", bufferCount);
}

void VulkanRenderPass::DestroyFramebuffers() {
  for (VkFramebuffer& framebuffer : _framebuffers) {
    vkDestroyFramebuffer(_device->GetLogicalDevice(), framebuffer, nullptr);
  }
}
}  // namespace Vulkan
}  // namespace Onyx
