#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	class COCOAPI IGraphicsResource
	{
	protected:
		IGraphicsResource() = default;
	public:
		virtual ~IGraphicsResource() = default;

		IGraphicsResource(const IGraphicsResource&) = delete;
		IGraphicsResource(IGraphicsResource&&) = delete;

		IGraphicsResource& operator=(const IGraphicsResource&) = delete;
		IGraphicsResource& operator=(IGraphicsResource&&) = delete;
	};
}