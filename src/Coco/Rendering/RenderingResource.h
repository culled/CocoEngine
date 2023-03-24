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
		RenderingResource(const string& name, ResourceType type);

	public:
		virtual ~RenderingResource() = default;

	protected:
		/// @brief Gets the rendering service's logger
		/// @return The rendering service's logger
		Logging::Logger* GetRenderingLogger() const;

		/// @brief Ensures that there is an active rendering service and returns it
		/// @return The active rendering service
		RenderingService* EnsureRenderingService() const;
	};
}