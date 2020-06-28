#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;
class VulkanRenderPass;

//! Contains information necessary to construct a new pipeline.
struct VulkanPipelineInfo {
  VkExtent2D Extent;
  VulkanRenderPass* RenderPass;
  std::vector<VkPipelineShaderStageCreateInfo> ShaderStageInfos;
  std::vector<VkDescriptorSetLayout> DescriptorSets;
};

//! Represents a Vulkan pipeline.
class VulkanPipeline final {
 public:
  VulkanPipeline(VulkanDevice* device, VulkanPipelineInfo pipelineInfo);
  ~VulkanPipeline();

  VkPipeline GetPipeline() { return _graphicsPipeline; }
  VkPipelineLayout GetPipelineLayout() { return _pipelineLayout; }

 private:
  void CreateLayout();
  void DestroyLayout();

  VulkanDevice* _device;
  VkPipelineLayout _pipelineLayout;
  VulkanPipelineInfo _pipelineInfo;
  VkPipeline _graphicsPipeline;
};
}  // namespace Vulkan
}  // namespace Onyx