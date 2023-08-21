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

		/// @brief The next open offset in the buffer
		uint64_t CurrentOffset = 0;

		/// @brief The amount of fragmented memory behind the current offset
		uint64_t FragmentedSize = 0;

		/// @brief The mapped memory of the buffer (if it has been locked)
		char* MappedMemory = nullptr;

		/// @brief Gets the size of the available memory region
		/// @return The size of the free space past the current offset in the buffer
		uint64_t GetFreeRegionSize() const noexcept;
	};

	struct ShaderUniformBufferRegion
	{
		int BufferIndex = -1;
		uint64_t Offset = Math::MaxValue<uint64_t>();
		uint64_t Length = 0;
		bool Preserve = false;
		ResourceVersion Version = 0;

		void Invalidate();
		bool IsInvalid(uint64_t minimumSize) const;
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
		void InvalidateBufferRegions(uint bufferIndex);
		void InvalidateBufferRegion(ShaderUniformBufferRegion& region);
		void FindBufferRegion(uint64_t requiredSize, ShaderUniformBufferRegion& region);
		ShaderUniformBuffer& CreateAdditionalBuffer();

		/// @brief Destroys the descriptor pool
		void DestroyPool();
	};
}