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
}

VulkanRenderPass::~VulkanRenderPass() {
  vkDestroyRenderPass(_device->GetLogicalDevice(), _renderPass, nullptr);
}

VkExtent2D VulkanRenderPass::GetExtent() { return _swapchain->GetExtent(); }
}  // namespace Vulkan
}  // namespace Onyx
