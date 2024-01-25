#pragma once

#include "../Entity.h"
#include "../SceneSystem/SceneSystem.h"
#include "../Components/NativeScriptComponent.h"

namespace Coco::ECS
{
	/// @brief Provides update functionality for NativeScripts
	class NativeScriptSystem : 
		public SceneSystem
	{
	public:
		static const int sSetupPriority;
		static const int sTickPriority;

	public:
		NativeScriptSystem(SharedRef<Scene> scene);

		// Inherited via SceneSystem
		int GetSetupPriority() const override { return sSetupPriority; }
		void SimulationStarted() override;
		void SimulationEnded() override;

		void SimulationTick(const TickInfo& tickInfo);
	};
}