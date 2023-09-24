#pragma once

#include "CachedVulkanResource.h"
#include "../VulkanBuffer.h"
#include "../VulkanDescriptorSetLayout.h"
#include "VulkanRenderPassShader.h"
#include "../VulkanCommandBuffer.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief Holds shader uniform data
	struct UniformDataBuffer
	{
		/// @brief The buffer
		Ref<VulkanBuffer> Buffer;

		/// @brief The mapped memory of the buffer (if it has been locked)
		char* MappedMemory;

		UniformDataBuffer(Ref<VulkanBuffer> buffer);
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
	class VulkanUniformData : 
		public CachedVulkanResource
	{
	public:
		/// @brief The ID of the global instance
		static const uint64 sGlobalInstanceID;

	private:
		using DescriptorPoolList = std::list<VulkanDescriptorPool>;

		static constexpr uint64 _sBufferSize = 1024 * 10; // 10 KiB
		static constexpr uint64 _sMaxSets = 500;

		uint64 _version;
		UniformDataBufferList _uniformBuffers;
		VulkanDescriptorPoolCreateInfo _poolCreateInfo;
		DescriptorPoolList _pools;
		std::unordered_map<uint64, AllocatedUniformData> _uniformData;
		std::unordered_map<uint64, VkDescriptorSet> _allocatedSets;

	public:
		VulkanUniformData(const VulkanRenderPassShader& passShader);
		~VulkanUniformData();

		/// @brief Creates a key for a VulkanUniformData object
		/// @param passShader The shader
		/// @return The key
		static GraphicsDeviceResourceID MakeKey(const VulkanRenderPassShader& passShader);

		/// @brief Gets this resource's version
		/// @return The version
		uint64 GetVersion() const { return _version; }

		/// @brief Prepares uniform data for a given instance
		/// @param instanceID The id of the instance
		/// @param uniformData The uniform data
		/// @param shader The shader
		/// @param preserve If true, the uniform data will be preserved between renders
		/// @return A descriptor set that can be used to bind the uniform data, or nullptr if no descriptors are needed
		VkDescriptorSet PrepareData(uint64 instanceID, const ShaderUniformData& uniformData, const VulkanRenderPassShader& shader, bool preserve);

		/// @brief Prepares push constant uniform data
		/// @param commandBuffer The command buffer
		/// @param pipelineLayout The pipeline layout
		/// @param uniformData The uniform data
		/// @param shader The shader
		void PreparePushConstants(VulkanCommandBuffer& commandBuffer, VkPipelineLayout pipelineLayout, const ShaderUniformData& uniformData, const VulkanRenderPassShader& shader);

		/// @brief Resets this object for a new frame
		void ResetForNextFrame();

		/// @brief Determines if this uniform data needs to be updated
		/// @param passShader The shader
		/// @return True if this resource should be updated
		bool NeedsUpdate(const VulkanRenderPassShader& passShader) const;

		/// @brief Updates this uniform data to be compatible with the given shader
		/// @param passShader The shader
		void Update(const VulkanRenderPassShader& passShader);

	private:
		/// @brief Gets the scope of an instance. Will only be global for the global instance ID
		/// @param instanceID The instance ID
		/// @return The scope
		constexpr static UniformScope GetInstanceScope(uint64 instanceID) { return instanceID == sGlobalInstanceID ? UniformScope::Global : UniformScope::Instance; }

		/// @brief Gets buffer-friendly uniform data
		/// @param scope The scope of the data
		/// @param data The uniform data
		/// @param shader The shader
		/// @return Buffer-friendly uniform data
		std::vector<uint8> GetBufferData(UniformScope scope, const ShaderUniformData& data, const RenderPassShader& shader) const;

		/// @brief Allocates a region in the uniform buffers
		/// @param requiredSize The required size of the region, in bytes
		/// @param data The data that will be changed
		void AllocateBufferRegion(uint64 requiredSize, AllocatedUniformData& data);

		/// @brief Frees the data of a uniform block
		/// @param data The data to free
		void FreeBufferRegion(AllocatedUniformData& data);
		void FreeAllBufferRegions();

		VulkanDescriptorPool& CreateDescriptorPool();
		void DestroyDescriptorPool(const VulkanDescriptorPool& pool);
		void DestroyDescriptorPools();

		VkDescriptorSet AllocateDescriptorSet(uint64 instanceID, const VulkanDescriptorSetLayout& layout);

		bool UpdateDescriptorSet(
			UniformScope scope,
			const ShaderUniformData& uniformData, 
			const VulkanRenderPassShader& shader, 
			const AllocatedUniformData& data, 
			VkDescriptorSet& set);
	};
}