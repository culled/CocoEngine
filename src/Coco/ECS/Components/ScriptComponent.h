#pragma once

#include "../EntityComponent.h"
#include "../ECSService.h"

namespace Coco::ECS
{
	/// @brief Base class for custom script components
	class ScriptComponent : public EntityComponent
	{
		friend class Scene;

	public:
		ScriptComponent(const EntityID& owner);

		ScriptComponent(ScriptComponent&&) noexcept = default;
		ScriptComponent& operator=(ScriptComponent&&) noexcept = default;

		virtual ~ScriptComponent() = default;

	protected:
		/// @brief Called every tick
		/// @param deltaTime The time since the last tick was called
		virtual void Tick(double deltaTime) {}
	};
}