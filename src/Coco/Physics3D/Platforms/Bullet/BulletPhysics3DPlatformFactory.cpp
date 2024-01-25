#include "Phys3Dpch.h"
#include "BulletPhysics3DPlatformFactory.h"
#include "BulletPhysics3DPlatform.h"

namespace Coco::Physics3D::Bullet
{
	UniqueRef<Physics3DPlatform> BulletPhysics3DPlatformFactory::Create() const
	{
		return CreateUniqueRef<BulletPhysics3DPlatform>();
	}
}