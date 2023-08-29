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

	/// @brief Data for a descriptor write
	struct DescriptorWriteData
	{
		List<ShaderUniformDescriptor> Uniforms;
		List<ShaderTextureSampler> Samplers;
		UnorderedMap<string, ResourceID> Textures;
		Ref<BufferVulkan> Buffer;
		uint64_t BufferOffset;
		uint64_t BufferLength;

		DescriptorWriteData(
			ShaderDescriptorScope scope, 
			const Subshader& subshader, 
			const UnorderedMap<string, ResourceID> textures, 
			Ref<BufferVulkan> buffer, 
			uint64_t bufferOffset, 
			uint64_t bufferLength);
	};

	/// @brief A cached resource for a Vulkan shader
	class VulkanShaderResource : public GraphicsResource<GraphicsDeviceVulkan, RenderingResource>, public CachedResource
	{
	private:
		static constexpr uint _maxSets = 512;
		static const ResourceID _globalUBID;
		static constexpr uint64_t _bufferSize = 1024*10; // 10 KiB
		static constexpr double _fragmentFlushThreshold = 0.5;

		Ref<VulkanDescriptorPool> _pool;
		ShaderUniformBuffer _globalUniformBuffer;
		List<ShaderUniformBuffer> _instanceUniformBuffers;
		UnorderedMap<ResourceID, ShaderUniformBufferRegion> _bufferRegions;
		UnorderedMap<ResourceID, VkDescriptorSet> _descriptorSets;

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

		/// @brief Prepares global scope uniform data and returns a descriptor set for the created data
		/// @param vulkanSubshader The subshader
		/// @param renderView The render view
		/// @param globalData The global uniform data
		/// @return A vulkan descriptor set if there were any global uniforms
		VkDescriptorSet PrepareGlobalData(const VulkanSubshader& vulkanSubshader, const RenderView& renderView, const ShaderUniformData& globalData);

		/// @brief Prepares instance scope uniform data and returns a descriptor set for the created data
		/// @param vulkanSubshader The subshader
		/// @param renderView The render view
		/// @param instanceData The instance uniform data
		/// @return A vulkan descriptor set if there were any instance uniforms
		VkDescriptorSet PrepareInstanceData(const VulkanSubshader& vulkanSubshader, const RenderView& renderView, const ShaderUniformData& instanceData);

		/// @brief Resets this resource for a new frame
		void ResetForNewFrame();

		/// @brief Flushes instance buffer changes
		void FlushBufferChanges();

	private:
		/// @brief Creates or updates uniform data in an instance buffer
		/// @param data The uniform data
		/// @param vulkanSubshader The subshader
		/// @return The region of the uniform data in the instance buffers
		ShaderUniformBufferRegion CreateOrUpdateInstanceBuffer(const ShaderUniformData& data, const VulkanSubshader& vulkanSubshader);

		/// @brief Writes descriptor set changes
		/// @param set The set to write changes to
		/// @param data The data to write
		/// @param renderView The render view
		void WriteDescriptorSet(VkDescriptorSet set, const DescriptorWriteData& data, const RenderView& renderView);

		/// @brief Frees all regions in an instance buffer
		/// @param bufferIndex The index of the buffer to free regions for
		void FreeBufferRegions(uint bufferIndex);

		/// @brief Frees a region in an instance buffer
		/// @param region The region to free
		void FreeBufferRegion(ShaderUniformBufferRegion& region);

		/// @brief Finds a region in an instance buffer to fit the requested amount of data
		/// @param requiredSize The size of the memory required
		/// @param outRegion Will be updated with the allocated region
		void FindBufferRegion(uint64_t requiredSize, ShaderUniformBufferRegion& outRegion);

		/// @brief Creates a uniform buffer of the given size
		/// @param size The size of the buffer
		/// @return The created uniform buffer
		ShaderUniformBuffer CreateBuffer(uint64_t size);

		List<char> GetUniformData(ShaderDescriptorScope scope, const ShaderUniformData& data, const Subshader& subshader) const;

		/// @brief Destroys the descriptor pool
		void DestroyPool();
	};
}