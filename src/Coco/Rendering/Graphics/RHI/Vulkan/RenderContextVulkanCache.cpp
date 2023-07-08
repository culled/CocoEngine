#include "RenderContextVulkanCache.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Rendering/Material.h>
#include "GraphicsDeviceVulkan.h"
#include "Resources/Cache/VulkanShader.h"
#include "Resources/Cache/VulkanShaderResource.h"
#include "Resources/Cache/VulkanMaterialResource.h"
#include "Resources/Cache/VulkanFramebuffer.h"
#include "Resources/VulkanDescriptorPool.h"
#include "Resources/BufferVulkan.h"
#include <Coco/Rendering/Graphics/RenderView.h>

namespace Coco::Rendering::Vulkan
{
	uint64_t MaterialBuffer::GetFreeRegionSize() const noexcept
	{
		return Buffer->GetSize() - CurrentOffset;
	}

	RenderContextVulkanCache::RenderContextVulkanCache() : 
		BaseGraphicsResource<GraphicsDeviceVulkan>()
	{
		_shaderCache = CreateManagedRef<ResourceCache<VulkanShaderResource>>(RenderingService::DefaultGraphicsResourceTickLifetime);
		_materialCache = CreateManagedRef<ResourceCache<VulkanMaterialResource>>(RenderingService::DefaultGraphicsResourceTickLifetime);
		_framebufferCache = CreateManagedRef<ResourceCache<VulkanFramebuffer>>(RenderingService::DefaultGraphicsResourceTickLifetime);
	}

	RenderContextVulkanCache::~RenderContextVulkanCache()
	{
		_materialUBOs.Clear();
	}

	VulkanShaderResource* RenderContextVulkanCache::GetOrCreateShaderResource(VulkanShader* shader)
	{
		const ResourceID id = shader->ID;
		VulkanShaderResource* resource;

		if (!_shaderCache->Has(id))
			resource = _shaderCache->Create(id, shader);
		else
			resource = _shaderCache->Get(id);

		Assert(resource->IsValid());

		if (resource->NeedsUpdate(shader))
			resource->Update(shader);

		return resource;
	}

	VulkanMaterialResource* RenderContextVulkanCache::GetOrCreateMaterialResource(const MaterialRenderData& materialData)
	{
		const ResourceID id = materialData.ID;

		VulkanMaterialResource* resource;

		if (!_materialCache->Has(id))
			resource = _materialCache->Create(id, materialData);
		else
			resource = _materialCache->Get(id);

		// Update the material data if needed
		if (resource->NeedsUpdate(materialData))
		{
			// Choose a new place in the buffers if this resource needs to expand
			if (materialData.UniformData.Count() > resource->GetSize())
			{
				if (resource->GetBufferIndex() != Math::MaxValue<uint>() && _materialUBOs.Count() > 0)
				{
					// The old region is now fragmented
					_materialUBOs[resource->GetBufferIndex()].FragmentedSize += resource->GetSize();
				}

				// Find a new buffer region
				FindBufferRegion(resource, materialData);

				Assert(resource->GetBufferIndex() != Math::MaxValue<uint>());
				Assert(resource->GetBufferIndex() != Math::MaxValue<uint64_t>());
			}

			MaterialBuffer& buffer = _materialUBOs[resource->GetBufferIndex()];

			// Map the buffer data if it isn't already
			if (buffer.MappedMemory == nullptr)
				buffer.MappedMemory = reinterpret_cast<uint8_t*>(buffer.Buffer->Lock(0, _materialBufferSize));

			resource->Update(buffer.MappedMemory, materialData);
		}

		return resource;
	}

	VulkanFramebuffer* RenderContextVulkanCache::GetOrCreateFramebuffer(const Ref<RenderView>& renderView, VulkanRenderPass* renderPass, Ref<RenderPipeline> pipeline)
	{
		const ResourceID id = pipeline->ID;
		VulkanFramebuffer* resource;

		if (!_framebufferCache->Has(id))
			resource = _framebufferCache->Create(id, pipeline);
		else
			resource = _framebufferCache->Get(id);

		Assert(resource->IsValid());

		if (resource->NeedsUpdate(renderView))
			resource->Update(renderView, renderPass);

		return resource;
	}

	void RenderContextVulkanCache::FlushMaterialChanges()
	{
		for (int i = 0; i < _materialUBOs.Count(); i++)
		{
			MaterialBuffer& buffer = _materialUBOs[i];

			// Flush the buffer if it's mapped
			if(buffer.MappedMemory != nullptr)
				buffer.Buffer->Unlock();

			buffer.MappedMemory = nullptr;

			// If the buffer is too fragmented, invalidate it to cause a reshuffling of data
			if (static_cast<double>(buffer.FragmentedSize) / buffer.Buffer->GetSize() > _fragmentFlushThreshold)
			{
				LogInfo(_device->GetLogger(), FormattedString("Material buffer {} is too fragmented. Invalidating for new frame...", i));
				InvalidateMaterialBufferRegions(i);
			}
		}
	}

	void RenderContextVulkanCache::FreeDescriptorSets()
	{
		for (auto& shader : *_shaderCache)
			shader.GetPool()->FreeSets();
	}

	Ref<BufferVulkan> RenderContextVulkanCache::GetMaterialBuffer(uint bufferIndex) const noexcept
	{
		return _materialUBOs[bufferIndex].Buffer;
	}

	void RenderContextVulkanCache::PurgeResources() noexcept
	{
		// Purge shader cache
		uint64_t shadersPurged = _shaderCache->PurgeStaleResources();

		// Purge material cache
		uint64_t materialsPurged = _materialCache->PurgeStaleResources();

		if (shadersPurged > 0 || materialsPurged > 0)
			LogTrace(_device->GetLogger(), FormattedString("Purged {} cached shaders and {} cached materials from RenderContext cache", shadersPurged, materialsPurged));
	}

	void RenderContextVulkanCache::InvalidateMaterialBufferRegions(uint bufferIndex)
	{
		MaterialBuffer& buffer = _materialUBOs[bufferIndex];

		for (auto& material : *_materialCache)
		{
			if (material.GetBufferIndex() != bufferIndex)
				continue;

			material.InvalidateBufferRegion();
		}

		buffer.CurrentOffset = 0;
		buffer.FragmentedSize = 0;
	}

	void RenderContextVulkanCache::FindBufferRegion(VulkanMaterialResource* resource, const MaterialRenderData& materialData)
	{
		int freeBufferIndex = -1;
		uint64_t requiredSize = materialData.UniformData.Count();

		// Try to find a buffer that can fit this material data
		for (int i = 0; i < _materialUBOs.Count(); i++)
		{
			if (_materialUBOs[i].GetFreeRegionSize() >= requiredSize)
			{
				freeBufferIndex = i;
				break;
			}
		}

		// Make a new buffer if we can't fit the material in the current one
		if (freeBufferIndex == -1)
		{
			CreateAdditionalMaterialBuffer();
			LogInfo(_device->GetLogger(), FormattedString("Created addition material buffer. New buffer count is {}", _materialUBOs.Count()));
			freeBufferIndex = static_cast<int>(_materialUBOs.Count()) - 1;
		}

		Assert(freeBufferIndex != -1);

		resource->UpdateRegion(static_cast<uint>(freeBufferIndex), _materialUBOs[freeBufferIndex].CurrentOffset, requiredSize);
		_materialUBOs[freeBufferIndex].CurrentOffset += requiredSize;
	}

	void RenderContextVulkanCache::CreateAdditionalMaterialBuffer()
	{
		MaterialBuffer buffer = {};
		buffer.Buffer = _device->CreateResource<BufferVulkan>("Material Buffer",
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			_materialBufferSize,
			true);

		_materialUBOs.Add(buffer);
	}
}
