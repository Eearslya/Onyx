#include "pch.h"

#include "VulkanShader.h"

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
  CreateModules(hasVertex, hasFragment, hasGeometry, hasCompute);
  CreatePipeline();
}

VulkanShader::~VulkanShader() {
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

  _pipeline = new VulkanPipeline(_device, info);
}

void VulkanShader::DestroyPipeline() { delete _pipeline; }

void VulkanShader::DestroyModules() {
  delete _computeShader;
  delete _geometryShader;
  delete _fragmentShader;
  delete _vertexShader;
}
}  // namespace Vulkan
}  // namespace Onyx
