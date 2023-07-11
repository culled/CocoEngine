#include "ScriptComponent.h"
#include <Coco/Core/Engine.h>
#include "../Scene.h"

namespace Coco::ECS
{
	ScriptComponent::ScriptComponent(EntityID owner) : EntityComponent(owner)
	{}
}