#include "ScriptComponent.h"
#include <Coco/Core/Engine.h>
#include "../Scene.h"

namespace Coco::ECS
{
	ScriptComponent::ScriptComponent(const EntityID& owner) : EntityComponent(owner)
	{}
}