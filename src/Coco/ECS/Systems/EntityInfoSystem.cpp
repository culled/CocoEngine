#include "ECSpch.h"
#include "EntityInfoSystem.h"

#include "../Entity.h"
#include "../Components/EntityInfoComponent.h"

namespace Coco::ECS
{
	bool EntityInfoSystem::IsEntityVisible(const Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		if (!info.IsActive)
			return false;

		if (entity.HasParent())
		{
			return IsEntityVisible(entity.GetParent());
		}

		return true;
	}
}