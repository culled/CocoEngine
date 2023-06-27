#pragma once

#include <Coco/Rendering/RenderingResource.h>
#include <Coco/Core/Resources/CachedResource.h>

#include <Coco/Rendering/Graphics/RenderView.h>

namespace Coco::Rendering::Vulkan
{
	/// @brief A cached material resource
	class VulkanMaterialResource final : public RenderingResource, public CachedResource
	{
		uint _bufferIndex = Math::MaxValue<uint>();
		uint64_t _offset = Math::MaxValue<uint64_t>();
		uint64_t _size = 0;

	public:
		VulkanMaterialResource(ResourceID id, const string& name, uint64_t lifetime, const MaterialRenderData& materialData);
		~VulkanMaterialResource() = default;

		DefineResourceType(VulkanMaterialResource)

		bool IsValid() const noexcept final { return true; }
		bool NeedsUpdate() const noexcept final { return false; }

		bool NeedsUpdate(const MaterialRenderData& materialData) const noexcept;

		/// @brief Updates this cached material's region of data in the material buffer
		/// @param bufferMemory The memory of the buffer
		/// @param materialData The material data
		/// @param materialDataLength The size of the material data
		void Update(uint8_t* bufferMemory, const MaterialRenderData& materialData);

		uint GetBufferIndex() const { return _bufferIndex; }
		uint64_t GetOffset() const { return _offset; }
		uint64_t GetSize() const { return _size; }

		void UpdateRegion(uint bufferIndex, uint64_t offset, uint64_t size);

		/// @brief Invalidates this cached material's region of data
		void InvalidateBufferRegion();
	};
}