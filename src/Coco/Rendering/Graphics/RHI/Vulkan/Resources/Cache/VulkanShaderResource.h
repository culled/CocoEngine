#pragma once

#include <Coco/Rendering/Graphics/Resources/GraphicsResource.h>
#include <Coco/Core/Resources/CachedResource.h>
#include "../BufferVulkan.h"
#include "../../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;
	class VulkanShader;
	class VulkanSubshader;
	class VulkanDescriptorPool;

	/// @brief Holds shader uniform data
	struct ShaderUniformBuffer
	{
		/// @brief The buffer
		Ref<BufferVulkan> Buffer;

		/// @brief The mapped memory of the buffer (if it has been locked)
		char* MappedMemory = nullptr;
	};

	/// @brief Defines a region in a shader uniform buffer
	struct ShaderUniformBufferRegion
	{
		/// @brief The index of the buffer
		int BufferIndex = -1;

		/// @brief The version of the contents in this region
		ResourceVersion Version = 0;

		/// @brief The allocated block of the buffer
		FreelistAllocatedBlock AllocatedBlock;

		/// @brief If true, this region will be preserved after the frame is rendered
		bool Preserve = false;
	};

	/// @brief A cached resource for a Vulkan shader
	class VulkanShaderResource : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>, public CachedResource
	{
	private:
		static constexpr uint _maxSets = 512;
		static constexpr uint64_t _bufferSize = 1024*10; // 10 KiB
		static constexpr double _fragmentFlushThreshold = 0.5;

		Ref<VulkanDescriptorPool> _pool;
		List<ShaderUniformBuffer> _uniformBuffers;
		UnorderedMap<ResourceID, ShaderUniformBufferRegion> _bufferRegions;

	public:
		VulkanShaderResource(const ResourceID& id, const string& name, const VulkanShader& shader);
		~VulkanShaderResource();

		DefineResourceType(VulkanShaderResource)

		bool IsValid() const noexcept final { return true; }
		bool NeedsUpdate() const noexcept final { return false; }

		/// @brief Determines if this resource needs to be updated to reflect the given shader
		/// @param shader The shader
		/// @return True if this resource should be updated
		bool NeedsUpdate(const VulkanShader& shader) const noexcept;

		/// @brief Updates this cached resource
		/// @param shader The Vulkan shader to update with
		void Update(const VulkanShader& shader);

		/// @brief Gets the descriptor pool
		/// @return The descriptor pool
		Ref<VulkanDescriptorPool> GetPool() { return _pool; }

		std::pair<const ShaderUniformBufferRegion&, VkDescriptorSet> CreateOrUpdateBuffer(const ShaderUniformData& data, const VulkanSubshader& vulkanSubshader);

		Ref<BufferVulkan> GetUniformBuffer(const ShaderUniformBufferRegion& region);
		void FlushBufferChanges();

	private:
		void FreeBufferRegions(uint bufferIndex);
		void FreeBufferRegion(ShaderUniformBufferRegion& region);
		void FindBufferRegion(uint64_t requiredSize, ShaderUniformBufferRegion& region);
		ShaderUniformBuffer& CreateAdditionalBuffer();

		/// @brief Destroys the descriptor pool
		void DestroyPool();
	};
}