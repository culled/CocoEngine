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
		VulkanMaterialResource(ResourceID id, const string& name, const MaterialRenderData& materialData);
		~VulkanMaterialResource() = default;

		DefineResourceType(VulkanMaterialResource)

		bool IsValid() const noexcept final { return true; }
		bool NeedsUpdate() const noexcept final { return false; }

		/// @brief Checks if this material resource is still valid for the given material data
		/// @param materialData The material data
		/// @return True if this material resource should be updated
		bool NeedsUpdate(const MaterialRenderData& materialData) const noexcept;

		/// @brief Updates this cached material's region of data in the material buffer
		/// @param bufferMemory The memory of the buffer
		/// @param materialData The material data
		void Update(uint8_t* bufferMemory, const MaterialRenderData& materialData);

		/// @brief Gets the index of the buffer that contains this material data
		/// @return The index of the buffer that contains this material data
		uint GetBufferIndex() const { return _bufferIndex; }

		/// @brief Gets the offset (in bytes) in the buffer of this material data
		/// @return The offset (in bytes) in the buffer of this material data
		uint64_t GetOffset() const { return _offset; }

		/// @brief Gets the size (in bytes) of this material data
		/// @return The size (in bytes) of this material data
		uint64_t GetSize() const { return _size; }

		/// @brief Updates the region for this material data
		/// @param bufferIndex The index of the buffer
		/// @param offset The offset within the buffer
		/// @param size The size of this material data
		void UpdateRegion(uint bufferIndex, uint64_t offset, uint64_t size);

		/// @brief Invalidates this cached material's region of data
		void InvalidateBufferRegion();
	};
}