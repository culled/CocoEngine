#pragma once

#include <Coco/Core/Platform/EnginePlatformFactory.h>
#include "WindowsIncludes.h"

namespace Coco::Platforms::Win32
{
	/// @brief A factory that creates a Win32EnginePlatform
	class Win32EnginePlatformFactory : public EnginePlatformFactory
	{
	public:
		Win32EnginePlatformFactory(HINSTANCE hInstance);

		UniqueRef<EnginePlatform> Create() const final;

	private:
		HINSTANCE _hInstance;
	};
}