#include "ECSService.h"

namespace Coco::ECS
{
	ECSService* ECSService::_instance = nullptr;

	ECSService::ECSService(EngineServiceManager* serviceManager) : EngineService(serviceManager),
		_entityManager(CreateManaged<EntityManager>()),
		_componentManager(CreateManaged<EntityComponentManager>())
	{
		_instance = this;

		RegisterTickListener(this, &ECSService::Process, ProcessTickPriority);
	}

	void ECSService::Process(double deltaTime)
	{
		_entityManager->DestroyQueuedEntities();
	}
}