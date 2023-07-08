#pragma once

#include <Coco/Rendering/Graphics/Resources/CommandBuffer.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class CommandBufferPoolVulkan;
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a CommandBuffer
	class CommandBufferVulkan final : public GraphicsResource<GraphicsDeviceVulkan, CommandBuffer>
	{
	private:
		VkCommandBuffer _commandBuffer = nullptr;
		CommandBufferPoolVulkan* _pool;

	public:
		CommandBufferVulkan(ResourceID id, const string& name, bool isPrimary, CommandBufferPoolVulkan* pool);
		~CommandBufferVulkan() final;

		DefineResourceType(CommandBufferVulkan)

		/// @brief Gets the underlying Vulkan command buffer
		/// @return The underlying Vulkan command buffer
		VkCommandBuffer GetCmdBuffer() const noexcept { return _commandBuffer; }

	protected:
		void BeginImpl(bool isSingleUse, bool isSimultaneousUse) final;
		void EndImpl() override;
		void SubmitImpl(
			const List<GraphicsSemaphore*>& waitSemaphores,
			const List<GraphicsSemaphore*>& signalSemaphores,
			GraphicsFence* signalFence) final;
		void ResetImpl() final;
	};
}