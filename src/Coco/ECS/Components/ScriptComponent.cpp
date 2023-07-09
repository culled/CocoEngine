#include "ScriptComponent.h"
#include <Coco/Core/Engine.h>
#include "../Scene.h"

namespace Coco::ECS
{
	AttachedScript::AttachedScript() : _ecs(ECSService::Get()), _entityID(InvalidEntityID)
	{
		if (_ecs == nullptr)
			throw Exception("Could not find an active ECSService");
	}

	ScriptComponent::ScriptComponent(EntityID owner, ManagedRef<AttachedScript>&& script) : EntityComponent(owner),
		_script(std::move(script))
	{
		_script->_entityID = owner;
		_script->Start();
	}

	void ScriptComponent::Tick(double deltaTime)
	{
		if (!_script.IsValid())
			return;

		_script->Tick(deltaTime);
	}
}