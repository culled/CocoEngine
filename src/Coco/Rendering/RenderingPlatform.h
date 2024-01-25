#pragma once

#include "Renderpch.h"
#include <Coco/Core/Types/String.h>

namespace Coco::Rendering
{
	/// @brief Allows an EnginePlatform to support rendering
	class RenderingPlatform
	{
	public:
		/// @brief Gets GraphicsPlatform-specific rendering extensions
		/// @param renderRHIName The name of the render RHI being used
		/// @param includePresentationExtensions If true, extensions for presenting images should be included
		/// @param outExtensions The list of extensions to add to
		virtual void GetPlatformRenderingExtensions(const string& renderRHIName, bool includePresentationExtensions, std::vector<string>& outExtensions) const = 0;
	};
}