#include "Renderpch.h"
#include "RendererResource.h"

#include "RenderService.h"

namespace Coco::Rendering
{
	RendererResource::RendererResource(const ResourceID& id, const string& name) :
		Resource(id, name)
	{}

	RenderService* RendererResource::EnsureRenderService()
	{
		RenderService* rendering = RenderService::Get();
		if (!rendering)
			throw std::exception("No active RenderService was found");

		return rendering;
	}
}