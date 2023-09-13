#pragma once

#include "CPWinpch.h"

namespace Coco::Platforms::Win32
{
	/// @brief Provides rendering extensions to a Win32EnginePlatform
	class Win32RenderingExtensions
	{
	public:
		/// @brief Adds Windows-specific rendering extensions
		/// @param renderRHIName The name of the render RHI being used
		/// @param includePresentationExtensions If true, extensions for presenting images should be included
		/// @param outExtensions The list of extensions to add to
		virtual void GetRenderingExtensions(const char* renderRHIName, bool includePresentationExtensions, std::vector<const char*>& outExtensions) const;

	private:
#ifdef COCO_RENDERING_VULKAN
		/// @brief Adds Windows-specific Vulkan rendering extensions
		/// @param includePresentationExtensions If true, extensions for presenting images should be included
		/// @param outExtensions The list of extensions to add to
		void AddVulkanRenderingExtensions(bool includePresentationExtensions, std::vector<const char*>& outExtensions) const;
#endif
	};
}