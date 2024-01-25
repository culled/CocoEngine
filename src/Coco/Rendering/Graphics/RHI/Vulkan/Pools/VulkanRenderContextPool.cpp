#include "Renderpch.h"
#include "VulkanRenderContextPool.h"
#include "../VulkanGraphicsDevice.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanRenderContextPool::VulkanRenderContextPool(VulkanGraphicsDevice& device) :
		_device(device),
		_contexts()
	{}

	VulkanRenderContextPool::~VulkanRenderContextPool()
	{
		for (const Ref<VulkanRenderContext>& c : _contexts)
		{
			_device.TryReleaseResource(c->ID);
		}

		_contexts.clear();
	}

	Ref<VulkanRenderContext> VulkanRenderContextPool::Get()
	{
		for(Ref<VulkanRenderContext>& context : _contexts)
		{
			if (context.GetUseCount() > 2)
				continue;
		
			context->Reset();
			return context;
		}
		
		return _contexts.emplace_back(_device.CreateRenderContext());
	}
}