#include "VulkanMaterialResource.h"

namespace Coco::Rendering::Vulkan
{
	VulkanMaterialResource::VulkanMaterialResource(const ResourceID& id, const string& name, const MaterialRenderData& materialData) :
		RenderingResource(id, name), CachedResource(materialData.ID, materialData.Version)
	{
	}

	bool VulkanMaterialResource::NeedsUpdate(const MaterialRenderData& materialData) const noexcept
	{
		return _offset == Math::MaxValue<uint64_t>() || _bufferIndex == Math::MaxValue<uint>() || materialData.Version != GetReferenceVersion();
	}

	void VulkanMaterialResource::Update(uint8_t* bufferMemory, const MaterialRenderData& materialData)
	{
		uint8_t* dst = bufferMemory + _offset;
		std::memcpy(dst, materialData.UniformData.Data(), materialData.UniformData.Count());

		UpdateReferenceVersion(materialData.Version);
		IncrementVersion();
	}

	void VulkanMaterialResource::UpdateRegion(uint bufferIndex, uint64_t offset, uint64_t size)
	{
		_bufferIndex = bufferIndex;
		_offset = offset;
		_size = size;
	}

	void VulkanMaterialResource::InvalidateBufferRegion()
	{
		_offset = Math::MaxValue<uint64_t>();
		_size = 0;
	}
}
