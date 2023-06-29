#pragma once

#include "../EntityComponent.h"
#include "../ECSService.h"
#include <any>

namespace Coco::ECS
{
	class COCOAPI AttachedScript
	{
		friend class ScriptComponent;

	protected:
		ECSService* _ecs;
		EntityID _entityID;

	public:
		AttachedScript();
		virtual ~AttachedScript() = default;

	protected:
		virtual void Start() {}
		virtual void Tick(double deltaTime) {}
	};

	class ScriptComponent : public EntityComponent
	{
		friend class Scene;

	private:
		ManagedRef<AttachedScript> _script;

	public:
		ScriptComponent() = default;
		ScriptComponent(EntityID owner, ManagedRef<AttachedScript>&& script);

		ScriptComponent(ScriptComponent&&) noexcept = default;
		ScriptComponent& operator=(ScriptComponent&&) noexcept = default;

		virtual ~ScriptComponent() = default;

	private:
		void Tick(double deltaTime);
	};
}