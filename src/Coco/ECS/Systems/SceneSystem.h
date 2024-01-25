#pragma once

#include "../ECSpch.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/MainLoop/TickInfo.h>
#include "../SceneTickListener.h"
#include "../Scene.h"

namespace Coco::ECS
{
	class Entity;

	/// @brief A system that can perform component-based operations for a specific scene
	class SceneSystem
	{
	public:
		SceneSystem(Scene& scene);
		virtual ~SceneSystem() = default;

		virtual int GetSetupPriority() const = 0;

		virtual void EntitiesAdded(std::span<Entity> rootEntities) {}

		/// @brief Called when the scene begins simulation
		virtual void SimulationStarted() {}

		/// @brief Called when the scene ends simulation
		virtual void SimulationEnded() {}

	protected:
		Scene& _scene;

	protected:
		template<typename ObjectType>
		void RegisterTickListener(ObjectType* instance, void(ObjectType::* tickCallback)(const TickInfo&), int tickPriority)
		{
			_listeners.push_back(CreateManagedRef<SceneTickListener>(instance, tickCallback, tickPriority));
			_scene.AddTickListener(_listeners.back());
		}

	private:
		std::vector<ManagedRef<SceneTickListener>> _listeners;
	};
}