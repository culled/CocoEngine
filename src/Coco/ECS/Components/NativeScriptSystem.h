#pragma once

#include "../Entity.h"
#include "../SceneSystem.h"
#include "../Components/NativeScriptComponent.h"

namespace Coco::ECS
{
	/// @brief Provides update functionality for NativeScripts
	class NativeScriptSystem : 
		public SceneSystem
	{
	public:
		static const int sPriority;

	public:
		NativeScriptSystem(SharedRef<Scene> scene);

		// Inherited via SceneSystem
		int GetPriority() const override { return sPriority; }
		void Tick() override;
	};
}