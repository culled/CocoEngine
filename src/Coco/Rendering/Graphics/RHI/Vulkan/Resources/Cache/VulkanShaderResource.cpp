#include "VulkanShaderResource.h"

#include "VulkanShader.h"
#include "../VulkanDescriptorPool.h"
#include "../../GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
    VulkanShaderResource::VulkanShaderResource(ResourceID id, const string& name, uint64_t lifetime, const VulkanShader* shader) :
        GraphicsResource<GraphicsDeviceVulkan, RenderingResource>(id, name, lifetime), CachedResource(shader->GetID(), shader->GetVersion())
    {}

    bool VulkanShaderResource::NeedsUpdate(const VulkanShader* shader) const noexcept
    {
        return !_pool.IsValid() || shader->GetVersion() != GetOriginalVersion();
    }

	VulkanShaderResource::~VulkanShaderResource()
	{
		DestroyPool();
	}

	void VulkanShaderResource::Update(VulkanShader* shader)
	{
		DestroyPool();

		_pool = _device->CreateResource<VulkanDescriptorPool>(_maxSets, shader->GetDescriptorLayouts());
		UpdateOriginalVersion(shader->GetVersion());
		IncrementVersion();
	}

	void VulkanShaderResource::DestroyPool()
	{
		if (!_pool.IsValid())
			return;

		_device->PurgeResource(_pool);
	}
}
