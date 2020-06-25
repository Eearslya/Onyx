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
  VkExtent2D Extent;             //!< The extent of our pipeline.
  VulkanRenderPass* RenderPass;  //!< The render pass this pipeline will run in.
  std::vector<VkPipelineShaderStageCreateInfo>
      ShaderStageInfos;  //!< The shader stages this pipeline will execute.
};

//! Represents a Vulkan pipeline.
class VulkanPipeline final {
 public:
  //! Construct a new VulkanPipeline.
  /*!
    \param device The Vulkan device which owns this pipeline.
    \param pipelineInfo The information struct which dictates how to construct the pipeline.
  */
  VulkanPipeline(VulkanDevice* device, VulkanPipelineInfo pipelineInfo);
  ~VulkanPipeline();

  //! Get our pipeline handle.
  VkPipeline GetPipeline() { return _graphicsPipeline; }

 private:
  //! Create our Pipeline Layout.
  void CreateLayout();

  //! Destroy our Pipeline Layout.
  void DestroyLayout();

  VulkanDevice* _device;             //!< Our parent device.
  VkPipelineLayout _pipelineLayout;  //!< Our pipeline layut.
  VulkanPipelineInfo _pipelineInfo;  //!< Our pipeline information struct.
  VkPipeline _graphicsPipeline;      //!< Our pipeline handle.
};
}  // namespace Vulkan
}  // namespace Onyx