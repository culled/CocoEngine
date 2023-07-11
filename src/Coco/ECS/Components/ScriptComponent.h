#pragma once

#include "../EntityComponent.h"
#include "../ECSService.h"

namespace Coco::ECS
{
	class ScriptComponent : public EntityComponent
	{
		friend class Scene;

	public:
		ScriptComponent(EntityID owner);

		ScriptComponent(ScriptComponent&&) noexcept = default;
		ScriptComponent& operator=(ScriptComponent&&) noexcept = default;

		virtual ~ScriptComponent() = default;

	protected:
		virtual void Tick(double deltaTime) {}
	};
}