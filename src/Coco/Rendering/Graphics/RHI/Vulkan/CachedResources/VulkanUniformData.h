#pragma once

#include "../../../GraphicsDeviceResource.h"
#include "../VulkanBuffer.h"
#include "../VulkanDescriptorSetLayout.h"
#include "VulkanRenderPassShader.h"
#include "../VulkanCommandBuffer.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;

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

	struct VulkanDescriptorPoolCreateInfo
	{
		std::vector<VkDescriptorPoolSize> PoolSizes;
		VkDescriptorPoolCreateInfo CreateInfo;

		VulkanDescriptorPoolCreateInfo();
	};

	struct VulkanDescriptorPool
	{
		VkDescriptorPool Pool;
		double LastAllocateTime;

		VulkanDescriptorPool();
	};

	class VulkanUniformData : public GraphicsDeviceResource<VulkanGraphicsDevice>
	{
	public:
		static const uint64 sGlobalInstanceID;

	private:
		using DescriptorPoolList = std::list<VulkanDescriptorPool>;

		static constexpr uint64 _sBufferSize = 1024 * 10; // 10 KiB
		static constexpr uint64 _sMaxSets = 500;

		UniformDataBufferList _uniformBuffers;
		VulkanDescriptorPoolCreateInfo _poolCreateInfo;
		DescriptorPoolList _pools;
		std::unordered_map<uint64, AllocatedUniformData> _uniformData;
		std::unordered_map<uint64, VkDescriptorSet> _allocatedSets;
		std::vector<VulkanDescriptorSetLayout> _descriptorSetLayouts;
		double _lastUsedTime;

	public:
		VulkanUniformData(const VulkanRenderPassShader& passShader);
		~VulkanUniformData();

		static GraphicsDeviceResourceID MakeKey(const VulkanRenderPassShader& passShader);

		VkDescriptorSet PrepareData(uint64 instanceID, const ShaderUniformData& uniformData, const VulkanRenderPassShader& shader, bool preserve);
		void PreparePushConstants(VulkanCommandBuffer& commandBuffer, VkPipelineLayout pipelineLayout, const ShaderUniformData& uniformData, const VulkanRenderPassShader& shader);

		void ResetForNextRender();

		/// @brief Marks this framebuffer as used
		void Use();

		/// @brief Determines if this framebuffer is stale and can be purged
		/// @return True if this framebuffer can be purged
		bool IsStale() const;

	private:
		constexpr static UniformScope GetInstanceScope(uint64 instanceID) { return instanceID == sGlobalInstanceID ? UniformScope::Global : UniformScope::Instance; }
		std::vector<char> GetBufferData(UniformScope scope, const ShaderUniformData& data, const RenderPassShader& shader) const;

		void AllocateBufferRegion(uint64 requiredSize, AllocatedUniformData& data);
		void FreeBufferRegion(AllocatedUniformData& data);

		VulkanDescriptorPool& CreateDescriptorPool();
		void DestroyDescriptorPool(const VulkanDescriptorPool& pool);

		VkDescriptorSet AllocateDescriptorSet(uint64 instanceID, const VulkanDescriptorSetLayout& layout);

		bool UpdateDescriptorSet(
			UniformScope scope,
			const ShaderUniformData& uniformData, 
			const VulkanRenderPassShader& shader, 
			const AllocatedUniformData& data, 
			VkDescriptorSet& set);
	};
}