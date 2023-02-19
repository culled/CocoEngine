#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/Logger.h>

#include "GraphicsPlatforms.h"

namespace Coco::Rendering
{
	class RenderingService;

	struct GraphicsBackendCreationParameters
	{
		string ApplicationName;
		RenderingRHI RHI;
		bool SupportsPresentation = false;

		GraphicsBackendCreationParameters(const string& applicationName, RenderingRHI rhi) : ApplicationName(applicationName), RHI(rhi) {}
	};

	class COCOAPI GraphicsPlatform
	{
	public:
		const bool SupportsPresentation;

	protected:
		RenderingService* RenderService;

	public:
		GraphicsPlatform(RenderingService* renderingService, const GraphicsBackendCreationParameters& creationParams);
		virtual ~GraphicsPlatform() = default;

		static GraphicsPlatform* CreatePlatform(RenderingService* renderingService, const GraphicsBackendCreationParameters& creationParams);

		virtual Logging::Logger* GetLogger() const = 0;
		virtual RenderingRHI GetRHI() const = 0;
	};
}
