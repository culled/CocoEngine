#include "EntityComponentManager.h"

namespace Coco::ECS
{
	EntityComponentManager* EntityComponentManager::_instance = nullptr;

	EntityComponentManager::EntityComponentManager()
	{
		_instance = this;
	}
}
