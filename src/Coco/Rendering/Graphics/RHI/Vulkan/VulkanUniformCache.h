#pragma once
#include <Coco/Core/Types/Refs.h>
#include "VulkanBuffer.h"
#include "../../ShaderUniformLayout.h"

namespace Coco::Rendering::Vulkan
{
	class VulkanGraphicsDevice;
	class VulkanCommandBuffer;

	struct CachedVulkanUniformData
	{
		uint64 DataID;
		uint64 DataVersion;
		UniformScope Scope;
		Ref<VulkanBuffer> Buffer;
		std::optional<VulkanBufferAllocationBlock> AllocBlock;
		std::unordered_map<string, WeakSharedRef<Texture>> Textures;
		double LastUsageTime;

		CachedVulkanUniformData(uint64 dataID, UniformScope scope, Ref<VulkanBuffer> buffer);
		void Use();
	};

	class VulkanUniformCache
	{
	public:
		VulkanUniformCache(VulkanGraphicsDevice& device);
		~VulkanUniformCache();

		CachedVulkanUniformData& CreateOrGetData(
			uint64 dataID, 
			UniformScope dataScope, 
			const ShaderUniformLayout& layout);
		CachedVulkanUniformData& CreateOrUpdateData(
			uint64 dataID, 
			uint64 dataVersion, 
			UniformScope dataScope, 
			const ShaderUniformLayout& layout, 
			std::span<const ShaderUniformValue> values);

		bool TryGetCachedData(uint64 dataID, UniformScope dataScope, const CachedVulkanUniformData*& outData);
		bool DataNeedsUpdate(uint64 dataID, UniformScope dataScope, uint64 dataVersion) const;

		uint64 PurgeUnusedUniformData(double staleThreshold);

	private:
		VulkanGraphicsDevice& _device;

		Ref<VulkanBuffer> _globalUniformBuffer;
		Ref<VulkanBuffer> _instanceUniformBuffer;

		std::unordered_map<uint64, CachedVulkanUniformData> _globalCache;
		std::unordered_map<uint64, CachedVulkanUniformData> _instanceCache;

	private:
		const std::unordered_map<uint64, CachedVulkanUniformData>* GetCacheForScope(UniformScope dataScope) const;
		std::unordered_map<uint64, CachedVulkanUniformData>* GetCacheForScope(UniformScope dataScope);

		Ref<VulkanBuffer> GetBufferForScope(UniformScope dataScope);
		void EnsureCachedDataIsAllocated(uint64 dataSize, CachedVulkanUniformData& cachedData);
		void FreeCachedUniformData(CachedVulkanUniformData& cachedData);
		uint64 PurgeUniformCache(UniformScope dataScope, double staleThreshold);
	};
}