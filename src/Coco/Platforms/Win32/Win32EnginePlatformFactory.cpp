#include "CPWinpch.h"
#include "Win32EnginePlatformFactory.h"
#include "Win32EnginePlatform.h"

namespace Coco::Win32
{
	Win32EnginePlatformFactory::Win32EnginePlatformFactory(HINSTANCE hInstance) :
		_hInstance(hInstance)
	{}

	UniqueRef<EnginePlatform> Win32EnginePlatformFactory::Create() const
	{
		return CreateUniqueRef<Win32EnginePlatform>(_hInstance);
	}
}