#pragma once

#include <Coco/Core/Platform/EnginePlatformFactory.h>
#include "WindowsIncludes.h"

namespace Coco::Win32
{
	/// @brief A factory that creates a Win32EnginePlatform
	class Win32EnginePlatformFactory : public EnginePlatformFactory
	{
	private:
		HINSTANCE _hInstance;

	public:
		Win32EnginePlatformFactory(HINSTANCE hInstance);

		UniqueRef<EnginePlatform> Create() const final;
	};
}