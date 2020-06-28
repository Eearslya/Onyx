#include "pch.h"

#include "VulkanShader.h"

#define GLM_FORCE_RADIANS
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#include "Core/FileUtils.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanPipeline.h"
#include "Renderer/Vulkan/VulkanRenderPass.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanShaderModule::VulkanShaderModule(VulkanDevice* device, std::string sourceFile,
                                       ShaderStage stage)
    : _device(device), _stage(stage) {
  std::vector<char> source = FileUtils::ReadBinaryFile(sourceFile);
  VkShaderModuleCreateInfo moduleCreateInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  moduleCreateInfo.codeSize = source.size();
  moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(source.data());
  VK_CHECK(vkCreateShaderModule(_device->GetLogicalDevice(), &moduleCreateInfo, nullptr,
                                &_shaderModule));

  _pipelineCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  _pipelineCreateInfo.module = _shaderModule;
  _pipelineCreateInfo.pName = "main";

  switch (_stage) {
    case ShaderStage::Vertex:
      _pipelineCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
      break;
    case ShaderStage::Fragment:
      _pipelineCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      break;
    case ShaderStage::Geometry:
      _pipelineCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
      break;
    case ShaderStage::Compute:
      _pipelineCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
      break;
  }
}

VulkanShaderModule::~VulkanShaderModule() {
  vkDestroyShaderModule(_device->GetLogicalDevice(), _shaderModule, nullptr);
}

VulkanShader::VulkanShader(VulkanDevice* device, const char* shaderName,
                           VulkanRenderPass* renderPass, bool hasVertex, bool hasFragment,
                           bool hasGeometry, bool hasCompute)
    : _device(device),
      _shaderName(shaderName),
      _renderPass(renderPass),
      _vertexShader(nullptr),
      _fragmentShader(nullptr),
      _geometryShader(nullptr),
      _computeShader(nullptr) {
  _stageCount =
      (hasVertex ? 1 : 0) + (hasFragment ? 1 : 0) + (hasGeometry ? 1 : 0) + (hasCompute ? 1 : 0);
  Logger::Debug("Initializing shader \"%s\" with %d modules: %s%s%s%s", shaderName, _stageCount,
                hasVertex ? "VERT " : "", hasFragment ? "FRAG " : "", hasGeometry ? "GEOM " : "",
                hasCompute ? "COMP" : "");
  CreateModules(hasVertex, hasFragment, hasGeometry, hasCompute);
  CreatePipeline();
  CreateUniformBuffers();
  CreateDescriptorPool();
  CreateDescriptorSets();
}

VulkanShader::~VulkanShader() {
  DestroyDescriptorPool();
  DestroyPipeline();
  DestroyModules();
}

void VulkanShader::CreateModules(bool hasVertex, bool hasFragment, bool hasGeometry,
                                 bool hasCompute) {
  std::string shaderBaseName = "assets/shaders/";
  shaderBaseName += _shaderName;
  if (hasVertex) {
    std::string source = shaderBaseName + ".vert.spv";
    _vertexShader = new VulkanShaderModule(_device, source, ShaderStage::Vertex);
  }
  if (hasFragment) {
    std::string source = shaderBaseName + ".frag.spv";
    _fragmentShader = new VulkanShaderModule(_device, source, ShaderStage::Fragment);
  }
  if (hasGeometry) {
    std::string source = shaderBaseName + ".geom.spv";
    _geometryShader = new VulkanShaderModule(_device, source, ShaderStage::Geometry);
  }
  if (hasCompute) {
    std::string source = shaderBaseName + ".comp.spv";
    _computeShader = new VulkanShaderModule(_device, source, ShaderStage::Compute);
  }
}

void VulkanShader::CreatePipeline() {
  Logger::Debug("Initializing shader pipeline \"%s\"", _shaderName);
  VulkanPipelineInfo info{};
  info.Extent = _renderPass->GetExtent();
  info.RenderPass = _renderPass;
  info.ShaderStageInfos.reserve(_stageCount);

  if (HasVertexStage()) {
    info.ShaderStageInfos.push_back(_vertexShader->GetPipelineCreateInfo());
  }
  if (HasFragmentStage()) {
    info.ShaderStageInfos.push_back(_fragmentShader->GetPipelineCreateInfo());
  }
  if (HasGeometryStage()) {
    info.ShaderStageInfos.push_back(_geometryShader->GetPipelineCreateInfo());
  }
  if (HasComputeStage()) {
    info.ShaderStageInfos.push_back(_computeShader->GetPipelineCreateInfo());
  }

  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = 0;
  layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layoutBinding.descriptorCount = 1;
  layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  descriptorLayoutInfo.bindingCount = 1;
  descriptorLayoutInfo.pBindings = &layoutBinding;

  m_DescriptorSetLayouts.resize(1);
  VK_CHECK(vkCreateDescriptorSetLayout(*_device, &descriptorLayoutInfo, nullptr,
                                       &m_DescriptorSetLayouts[0]));
  info.DescriptorSets = m_DescriptorSetLayouts;

  _pipeline = new VulkanPipeline(_device, info);
}

void VulkanShader::CreateUniformBuffers() {
  m_UniformBuffers.resize(s_DescriptorPoolSize);
  for (U32 i = 0; i < s_DescriptorPoolSize; i++) {
    m_UniformBuffers[i].Create(*_device, sizeof(UniformBufferObject));
  }
}

void VulkanShader::CreateDescriptorPool() {
  VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
  poolSize.descriptorCount = s_DescriptorPoolSize;  // TODO: Figure this value out intelligently

  VkDescriptorPoolCreateInfo poolCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
  poolCreateInfo.poolSizeCount = 1;
  poolCreateInfo.pPoolSizes = &poolSize;
  poolCreateInfo.maxSets = s_DescriptorPoolSize;  // TODO: Figure this value out intelligently

  VK_CHECK(vkCreateDescriptorPool(*_device, &poolCreateInfo, nullptr, &m_DescriptorPool));
}

void VulkanShader::CreateDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(s_DescriptorPoolSize, m_DescriptorSetLayouts[0]);
  VkDescriptorSetAllocateInfo allocInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
  allocInfo.descriptorPool = m_DescriptorPool;
  allocInfo.descriptorSetCount = s_DescriptorPoolSize;
  allocInfo.pSetLayouts = layouts.data();
  m_DescriptorSets.resize(s_DescriptorPoolSize);
  VK_CHECK(vkAllocateDescriptorSets(*_device, &allocInfo, m_DescriptorSets.data()));

  for (U32 i = 0; i < s_DescriptorPoolSize; i++) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_UniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet descriptorWrite{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    descriptorWrite.dstSet = m_DescriptorSets[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(*_device, 1, &descriptorWrite, 0, nullptr);
  }
}

void VulkanShader::DestroyDescriptorPool() {
  vkDestroyDescriptorPool(*_device, m_DescriptorPool, nullptr);
}

void VulkanShader::DestroyUniformBuffers() {
  for (VulkanUniformBuffer buffer : m_UniformBuffers) {
    buffer.Destroy(*_device);
  }
}

void VulkanShader::DestroyPipeline() {
  for (VkDescriptorSetLayout layout : m_DescriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(*_device, layout, nullptr);
  }
  delete _pipeline;
}

void VulkanShader::DestroyModules() {
  delete _computeShader;
  delete _geometryShader;
  delete _fragmentShader;
  delete _vertexShader;
}

void VulkanShader::UpdateUniformBuffer(U32 bufferIndex, UniformBufferObject& ubo) {
  m_UniformBuffers[bufferIndex].Upload(*_device, &ubo, sizeof(ubo));
}
}  // namespace Vulkan
}  // namespace Onyx
