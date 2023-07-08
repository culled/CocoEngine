#pragma once

#include <Coco/Core/Resources/Resource.h>

namespace Coco::Logging
{
	class Logger;
}

namespace Coco::Rendering
{
	class RenderingService;

	/// @brief A Resource that has access to the RenderingService
	class COCOAPI RenderingResource : public Resource
	{
	protected:
		RenderingResource(ResourceID id, const string& name);

	public:
		virtual ~RenderingResource() = default;

	protected:
		/// @brief Gets the rendering service's logger
		/// @return The rendering service's logger
		Logging::Logger* GetRenderingLogger();

		/// @brief Ensures that there is an active rendering service and returns it
		/// @return The active rendering service
		RenderingService* EnsureRenderingService();
	};
}