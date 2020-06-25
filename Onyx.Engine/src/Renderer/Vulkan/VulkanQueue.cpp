#include "pch.h"

#include "VulkanQueue.h"

#include "Renderer/Vulkan/VulkanCommandBuffer.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanUtilities.h"

namespace Onyx {
namespace Vulkan {
VulkanQueue::VulkanQueue(VulkanDevice* device, U32 index)
    : _device(device), _index(index), _queue(VK_NULL_HANDLE) {
  vkGetDeviceQueue(_device->GetLogicalDevice(), _index, 0, &_queue);
}

void VulkanQueue::Submit(VulkanCommandBuffer* buffer, std::vector<VkSemaphore> waitSemaphores,
                         std::vector<VkSemaphore> signalSemaphores,
                         VkFence signalFence) {
  VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  std::vector<VkCommandBuffer> buffers = {buffer->GetCommandBuffer()};
  submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
  submitInfo.pWaitSemaphores = waitSemaphores.data();
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
  submitInfo.pSignalSemaphores = signalSemaphores.data();
  submitInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());
  submitInfo.pCommandBuffers = buffers.data();
  VK_CHECK(vkQueueSubmit(_queue, 1, &submitInfo, signalFence));
}
void VulkanQueue::WaitIdle() { vkQueueWaitIdle(_queue); }
}  // namespace Vulkan
}  // namespace Onyx
