#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>

#include "RenderView.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A context that can be used for rendering
	/// </summary>
	class COCOAPI RenderContext
	{
	protected:
		Ref<RenderView> RenderView;

	protected:
		RenderContext(Ref<Rendering::RenderView> renderView);

	public:
		virtual ~RenderContext();

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void SetViewport(const Vector2Int& offset, const SizeInt& size) = 0;

		void RestoreViewport();
	};
}