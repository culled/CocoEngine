#include "VulkanShaderResource.h"

#include "VulkanShader.h"
#include "../VulkanDescriptorPool.h"
#include "../../GraphicsDeviceVulkan.h"

namespace Coco::Rendering::Vulkan
{
    VulkanShaderResource::VulkanShaderResource(const ResourceID& id, const string& name, const VulkanShader& shader) :
        GraphicsResource<GraphicsDeviceVulkan, RenderingResource>(id, name), 
		CachedResource(shader.ID, shader.GetVersion())
    {}

    bool VulkanShaderResource::NeedsUpdate(const VulkanShader& shader) const noexcept
    {
        return !_pool.IsValid() || shader.GetVersion() != GetReferenceVersion();
    }

	VulkanShaderResource::~VulkanShaderResource()
	{
		DestroyPool();
	}

	void VulkanShaderResource::Update(const VulkanShader& shader)
	{
		DestroyPool();

		_pool = _device->CreateResource<VulkanDescriptorPool>(FormattedString("{} Descriptor Pool", shader.GetName()), _maxSets, shader.GetDescriptorLayouts());
		UpdateReferenceVersion(shader.GetVersion());
		IncrementVersion();
	}

	void VulkanShaderResource::DestroyPool()
	{
		if (!_pool.IsValid())
			return;

		_device->PurgeResource(_pool);
		_pool = Ref<VulkanDescriptorPool>();
	}
}
