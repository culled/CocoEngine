#pragma once

#include "../EntityComponent.h"
#include "../ECSService.h"

namespace Coco::ECS
{
	class COCOAPI ScriptComponent : public EntityComponent
	{
		friend class Scene;

	protected:
		ECSService* _ecs;

	public:
		ScriptComponent() = default;
		ScriptComponent(EntityID owner);

		virtual ~ScriptComponent() = default;

	protected:
		virtual void Tick(double deltaTime) {}
	};
}