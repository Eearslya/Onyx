#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "Renderer/UniformBufferObject.h"
#include "Renderer/Vulkan/VulkanBuffer.h"

namespace Onyx {
namespace Vulkan {
class VulkanDevice;
class VulkanPipeline;
class VulkanRenderPass;

//! Describes at which stage this shader module should be executed.
enum class ShaderStage { Vertex = 0, Fragment, Geometry, Compute };

//! Represents a single shader module.
class VulkanShaderModule final {
 public:
  //! Construct a new VulkanShaderModule.
  /*!
    \param device The Vulkan device which owns this shader.
    \param sourceFile A path to a binary file containing compiled SPIR-V code.
    \param stage The stage at which to attach this shader module.
  */
  VulkanShaderModule(VulkanDevice* device, std::string sourceFile, ShaderStage stage);
  ~VulkanShaderModule();

  //! Returns the information necessary to bind this shader module when creating the pipeline.
  VkPipelineShaderStageCreateInfo& GetPipelineCreateInfo() { return _pipelineCreateInfo; }

 private:
  VulkanDevice* _device;                                //!< Our parent device.
  ShaderStage _stage;                                   //!< Our shader stage.
  VkPipelineShaderStageCreateInfo _pipelineCreateInfo;  //!< Our pipeline creation info.
  VkShaderModule _shaderModule;                         //!< Handle to our Vulkan shader module.
};

class VulkanShader final {
 public:
  VulkanShader(VulkanDevice* device, const char* shaderName, VulkanRenderPass* renderPass,
               bool hasVertex = true, bool hasFragment = true, bool hasGeometry = false,
               bool hasCompute = false);
  ~VulkanShader();

  void UpdateUniformBuffer(U32 bufferIndex, UniformBufferObject& ubo);

  VulkanPipeline* GetPipeline() { return _pipeline; }
  VkDescriptorSet GetDescriptorSet(U32 setIndex) { return m_DescriptorSets[setIndex]; }

  const bool HasVertexStage() const { return _vertexShader != nullptr; }
  const bool HasFragmentStage() const { return _fragmentShader != nullptr; }
  const bool HasGeometryStage() const { return _geometryShader != nullptr; }
  const bool HasComputeStage() const { return _computeShader != nullptr; }

 private:
  void CreateModules(bool hasVertex, bool hasFragment, bool hasGeometry, bool hasCompute);
  void CreatePipeline();
  void CreateUniformBuffers();
  void CreateDescriptorPool();
  void CreateDescriptorSets();
  void DestroyDescriptorPool();
  void DestroyUniformBuffers();
  void DestroyPipeline();
  void DestroyModules();

  VulkanDevice* _device;
  const char* _shaderName;
  VulkanRenderPass* _renderPass;
  U32 _stageCount;
  VulkanShaderModule* _vertexShader;
  VulkanShaderModule* _fragmentShader;
  VulkanShaderModule* _geometryShader;
  VulkanShaderModule* _computeShader;
  VulkanPipeline* _pipeline;
  VkDescriptorPool m_DescriptorPool;
  std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
  std::vector<VkDescriptorSet> m_DescriptorSets;
  std::vector<VulkanUniformBuffer> m_UniformBuffers;

  static const U32 s_DescriptorPoolSize = 10;
};
}  // namespace Vulkan
}  // namespace Onyx