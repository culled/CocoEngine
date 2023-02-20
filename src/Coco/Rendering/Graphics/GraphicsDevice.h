#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Version.h>

#include "GraphicsPlatformTypes.h"

namespace Coco::Rendering
{
	struct COCOAPI GraphicsDeviceCreationParameters
	{
		GraphicsDeviceType PreferredDeviceType = GraphicsDeviceType::Discrete;
		bool SupportsPresentation = true;

		bool RequireGraphicsCapability = true;
		bool RequireComputeCapability = true;
		bool RequireTransferCapability = true;

		List<string> RequiredExtensions;
	};

	class COCOAPI GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() = default;

		virtual string GetName() const = 0;
		virtual GraphicsDeviceType GetType() const = 0;
		virtual Version GetDriverVersion() const = 0;
		virtual Version GetAPIVersion() const = 0;
	};
}

