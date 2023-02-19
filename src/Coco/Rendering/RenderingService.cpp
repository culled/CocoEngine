#include "RenderingService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderingService::RenderingService(const GraphicsBackendCreationParameters& backendCreateParams)
	{
		_graphics.reset(GraphicsPlatform::CreatePlatform(this, backendCreateParams));
	}

	RenderingService::~RenderingService()
	{
		_graphics.reset();
	}

	Logging::Logger* RenderingService::GetLogger() const
	{
		return Engine::Get()->GetLogger();
	}

	void RenderingService::Start()
	{
	}

	void RenderingService::Render(const RenderView& view, const RenderPipeline& pipeline)
	{
	}
}