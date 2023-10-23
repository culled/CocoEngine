#pragma once

#include <Coco/Core/Types/Refs.h>

namespace Coco::ECS
{
	class Scene;

	/// @brief A system that can perform component-based operations for a specific scene
	class SceneSystem
	{
	public:
		virtual ~SceneSystem() = default;

		/// @brief Gets the execution priority for this system.
		/// Systems with lower priorities are executed before those with higher priorities
		/// @return This system's priority
		virtual int GetPriority() const = 0;

		/// @brief Executes this system for a given scene
		/// @param scene The scene to execute
		virtual void Execute(SharedRef<Scene> scene) = 0;
	};
}