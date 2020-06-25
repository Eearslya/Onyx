#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"

#include <vulkan/vulkan.h>

#include <string>

namespace Onyx {
namespace Vulkan {
class VulkanDevice;

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

//! Represents a full shader suite.
class VulkanShader final {
 public:
  //! Construct a new VulkanShader.
  /*!
    \param device The Vulkan device which owns this shader.
    \param shaderName The name of the shader to load.
    \param hasVertex Indicates whether this shader has a Vertex stage.
    \param hasFragment Indicates whether this shader has a Fragment stage.
    \param hasGeometry Indicates whether this shader has a Geometry stage.
    \param hasCompute Indicates whether this shader has a Compute stage.
  */
  VulkanShader(VulkanDevice* device, const char* shaderName, bool hasVertex = true,
               bool hasFragment = true, bool hasGeometry = false, bool hasCompute = false);
  ~VulkanShader();

  //! Indicates whether this shader has a Vertex stage.
  const bool HasVertexStage() const { return _vertexShader != nullptr; }

  //! Indicates whether this shader has a Fragment stage.
  const bool HasFragmentStage() const { return _fragmentShader != nullptr; }

  //! Indicates whether this shader has a Geometry stage.
  const bool HasGeometryStage() const { return _geometryShader != nullptr; }

  //! Indicates whether this shader has a Compute stage.
  const bool HasComputeStage() const { return _computeShader != nullptr; }

 private:
  //! Create and store all of our shader modules.
  void CreateModules(bool hasVertex, bool hasFragment, bool hasGeometry, bool hasCompute);

  //! Destroy and clean up all of our shader modules.
  void DestroyModules();

  VulkanDevice* _device;                //!< Our parent device.
  const char* _shaderName;              //!< Our shader name.
  VulkanShaderModule* _vertexShader;    //!< Our vertex shader, if applicable.
  VulkanShaderModule* _fragmentShader;  //!< Our fragment shader, if applicable.
  VulkanShaderModule* _geometryShader;  //!< Our geometry shader, if applicable.
  VulkanShaderModule* _computeShader;   //!< Our computer shader, if applicable.
};
}  // namespace Vulkan
}  // namespace Onyx