#pragma once

#include <Coco/Core/Resources/Resource.h>
#include "RenderingService.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A Resource that has access to the RenderingService
	/// </summary>
	class COCOAPI RenderingResource : public Resource
	{
	protected:
		RenderingResource() = default;

	public:
		virtual ~RenderingResource() = default;

	protected:
		/// <summary>
		/// Gets the rendering service's logger
		/// </summary>
		/// <returns>The rendering service's logger</returns>
		Logging::Logger* GetRenderingLogger() const;

		/// <summary>
		/// Ensures that there is an active rendering service and returns it
		/// </summary>
		/// <returns>The active rendering service</returns>
		RenderingService* EnsureRenderingService() const;
	};
}