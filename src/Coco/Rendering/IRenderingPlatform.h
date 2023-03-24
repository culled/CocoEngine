#pragma once

#include <Coco/Core/API.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/String.h>

namespace Coco::Platform
{
	/// @brief An interface that allows EnginePlatforms to render
	class COCOAPI IRenderingPlatform
	{
	public:
		/// @brief Called to fill out the neccessary rendering extensions for the given render hardware interface on the current platform
		/// @param renderingRHI The type of the render hardware interface being used
		/// @param includePresentationExtensions If true, extensions for presenting will be included
		/// @param extensionNames A list that will be populated with the required render extensions
		virtual void GetRenderingExtensions(int renderingRHI, bool includePresentationExtensions, List<string>& extensionNames) const noexcept = 0;
	};
}