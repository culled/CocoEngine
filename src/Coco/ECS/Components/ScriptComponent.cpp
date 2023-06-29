#include "ScriptComponent.h"
#include <Coco/Core/Engine.h>
#include "../Scene.h"

namespace Coco::ECS
{
	ScriptComponent::ScriptComponent()
	{
		Engine* engine = Engine::Get();

		if (!engine->GetServiceManager()->TryFindService<ECSService>(_ecs))
			throw Exception("Could not find an active ECSService");
	}

	ScriptComponent::ScriptComponent(EntityID owner) : EntityComponent(owner)
	{
		Engine* engine = Engine::Get();

		if (!engine->GetServiceManager()->TryFindService<ECSService>(_ecs))
			throw Exception("Could not find an active ECSService");
	}
}