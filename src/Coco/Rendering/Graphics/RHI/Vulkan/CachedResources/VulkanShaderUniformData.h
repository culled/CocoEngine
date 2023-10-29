#pragma once

#include "CachedVulkanResource.h"
#include <Coco/Core/Types/Freelist.h>

#include "VulkanGlobalUniformData.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering
{
	struct ShaderVariant;
	struct ShaderUniformData;
}

namespace Coco::Rendering::Vulkan
{
	class VulkanBuffer;
	class VulkanShaderVariant;
	class VulkanCommandBuffer;
	class VulkanCommandBuffer;
	struct VulkanDescriptorSetLayout;
	class VulkanPipeline;

	/// @brief Holds shader uniform data
	struct UniformDataBuffer
	{
		/// @brief The buffer
		ManagedRef<VulkanBuffer> Buffer;

		/// @brief The mapped memory of the buffer (if it has been locked)
		char* MappedMemory;

		UniformDataBuffer(ManagedRef<VulkanBuffer>&& buffer);
	};

	using UniformDataBufferList = std::list<UniformDataBuffer>;

	/// @brief Defines a region in a shader uniform buffer
	struct AllocatedUniformData
	{
		/// @brief The identifier for the data
		uint64 ID;

		/// @brief The version of the contents in this region
		uint64 Version;

		/// @brief The buffer that is holding the uniform data
		UniformDataBufferList::iterator Buffer;

		/// @brief The allocated block of the buffer
		FreelistNode AllocatedBlock;

		/// @brief If true, this region will be preserved after the frame is rendered
		bool Preserve;

		AllocatedUniformData(uint64 id, bool preserve);
	};

	/// @brief Holds information to construct a Vulkan descriptor pool
	struct VulkanDescriptorPoolCreateInfo
	{
		/// @brief The pool sizes
		std::vector<VkDescriptorPoolSize> PoolSizes;

		/// @brief The create info
		VkDescriptorPoolCreateInfo CreateInfo;

		VulkanDescriptorPoolCreateInfo();
	};

	/// @brief A pool that can allocate descriptor sets
	struct VulkanDescriptorPool
	{
		/// @brief The pool
		VkDescriptorPool Pool;

		/// @brief The time that this pool was last allocated from
		double LastAllocateTime;

		VulkanDescriptorPool();
	};

	/// @brief Manages uniform data for a VulkanRenderContext
	class VulkanShaderUniformData : 
		public CachedVulkanResource
	{
	private:
		using DescriptorPoolList = std::list<VulkanDescriptorPool>;

		static constexpr uint64 _sBufferSize = 1024 * 10; // 10 KiB
		static constexpr uint64 _sMaxSets = 500;

		uint64 _version;
		std::optional<VulkanGlobalUniformData> _globalUniformData;
		UniformDataBufferList _uniformBuffers;
		VulkanDescriptorPoolCreateInfo _poolCreateInfo;
		DescriptorPoolList _pools;
		std::unordered_map<uint64, AllocatedUniformData> _uniformData;
		std::unordered_map<uint64, VkDescriptorSet> _instanceSets;
		std::vector<uint8> _uniformDataBuffer;

	public:
		VulkanShaderUniformData(const VulkanShaderVariant& shader);
		~VulkanShaderUniformData();

		/// @brief Makes a key from a shader
		/// @param shader The shader
		/// @return The key unique to the shader
		static GraphicsDeviceResourceID MakeKey(const VulkanShaderVariant& shader);

		/// @brief Gets this resource's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Determines if this uniform data needs to be updated
		/// @param shader The shader
		/// @return True if this resource should be updated
		bool NeedsUpdate(const VulkanShaderVariant& shader) const;

		/// @brief Updates this uniform data to be compatible with the given shader
		/// @param shader The shader
		void Update(const VulkanShaderVariant& shader);

		/// @brief Sets data for a buffer uniform
		/// @param key The uniform key
		/// @param dataOffset The offset into the buffer to start copying data
		/// @param data The data
		/// @param dataSize The size of the data
		void SetBufferUniformData(ShaderUniformData::UniformKey key, uint64 dataOffset, const void* data, uint64 dataSize);

		/// @brief Prepares the given global uniform data
		/// @param uniformData The uniform data
		/// @return A descriptor set if the data was prepared, else nullptr
		VkDescriptorSet PrepareGlobalData(const ShaderUniformData& uniformData);

		/// @brief Prepares the given instance uniform data
		/// @param shader The shader
		/// @param instanceID The instance ID
		/// @param uniformData The uniform data
		/// @param preserve If true, the data will be preserved between frames
		/// @return A descriptor set if the data was prepared, else nullptr
		VkDescriptorSet PrepareInstanceData(const VulkanShaderVariant& shader, uint64 instanceID, const ShaderUniformData& uniformData, bool preserve);

		/// @brief Prepares the given draw uniform data
		/// @param shader The shader
		/// @param commandBuffer The command buffer
		/// @param pipeline The current pipeline
		/// @param uniformData The uniform data
		/// @return A descriptor set if the data was prepared, else nullptr
		VkDescriptorSet PrepareDrawData(const VulkanShaderVariant& shader, VulkanCommandBuffer& commandBuffer, const VulkanPipeline& pipeline, const ShaderUniformData& uniformData);

		/// @brief Resets this object for a new frame
		void ResetForNextFrame();

	private:
		/// @brief Allocates a region in the uniform buffers
		/// @param requiredSize The required size of the region, in bytes
		/// @param data The data that will be changed
		void AllocateBufferRegion(uint64 requiredSize, AllocatedUniformData& data);

		/// @brief Frees the data of a uniform block
		/// @param data The data to free
		void FreeBufferRegion(AllocatedUniformData& data);

		/// @brief Frees all uniform buffer regions
		void FreeAllBufferRegions();

		/// @brief Creates a descriptor pool
		/// @return The descriptor pool
		VulkanDescriptorPool& CreateDescriptorPool();

		/// @brief Destroys a descriptor pool
		/// @param pool The pool to destroy
		void DestroyDescriptorPool(const VulkanDescriptorPool& pool);

		/// @brief Destroys all descriptor pools
		void DestroyDescriptorPools();

		/// @brief Allocates a new descriptor set
		/// @param instanceID The ID of the instance
		/// @param layout The descriptor set layout
		/// @return A descriptor set
		VkDescriptorSet AllocateDescriptorSet(const VulkanDescriptorSetLayout& layout);

		/// @brief Updates a descriptor set
		/// @param layout The uniform layout
		/// @param setLayout The descriptor set layout
		/// @param uniformData The uniform data
		/// @param data The allocated uniform data
		/// @param set The set that will be updated
		/// @return True if the set was successfully updated
		bool UpdateDescriptorSet(
			const ShaderUniformLayout& layout,
			const VulkanDescriptorSetLayout& setLayout,
			const ShaderUniformData& uniformData,
			const AllocatedUniformData* data,
			VkDescriptorSet& set);
	};
}