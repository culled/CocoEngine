#include "ECSpch.h"
#include "NativeScriptSystem.h"
#include "../SceneView.h"
#include "../Components/NativeScriptComponent.h"

#include <Coco/Core/MainLoop/MainLoop.h>

namespace Coco::ECS
{
	const int NativeScriptSystem::sPriority = 0;

	NativeScriptSystem::NativeScriptSystem(SharedRef<Scene> scene) :
		SceneSystem(scene)
	{}

	void NativeScriptSystem::Tick()
	{
		const TickInfo& tickInfo = MainLoop::cGet()->GetCurrentTick();

		SharedRef<Scene> scene = _scene.lock();
		SceneView<NativeScriptComponent> view(scene);

		for (Entity& entity : view)
		{
			NativeScriptComponent& nsc = entity.GetComponent<NativeScriptComponent>();

			if (!entity.IsActiveInHierarchy() || nsc._scriptStarted)
				continue;

			nsc._scriptInstance->OnStart();
			nsc._scriptStarted = true;
		}

		for (Entity& entity : view)
		{
			if (!entity.IsActiveInHierarchy())
				continue;

			NativeScriptComponent& nsc = entity.GetComponent<NativeScriptComponent>();
			nsc._scriptInstance->OnUpdate(tickInfo);
		}
	}
}