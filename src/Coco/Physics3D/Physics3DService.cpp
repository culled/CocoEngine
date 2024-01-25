#include "Phys3Dpch.h"
#include "Physics3DService.h"

namespace Coco::Physics3D
{
	Physics3DService::Physics3DService(const Physics3DPlatformFactory& platformFactory) :
		_platform(platformFactory.Create())
	{}

	Physics3DService::~Physics3DService()
	{
		_platform.reset();
	}
}