#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Rendering/RenderView.h>

namespace Coco::Rendering
{
	/// <summary>
	/// A context that can be used for rendering
	/// </summary>
	class COCOAPI RenderContext
	{
	protected:
		Managed<RenderView> RenderView;

	protected:
		RenderContext(Rendering::RenderView* renderView);

	public:
		virtual ~RenderContext();
	};
}