#include "ECSpch.h"
#include "SceneSystemFactory.h"
#include "SceneSystem.h"
#include "../Scene.h"
#include "../Entity.h"

namespace Coco::ECS
{
	std::vector<UniqueRef<SceneSystem>> SceneSystemFactory::CreateSystems(Scene& scene)
	{
		std::vector<UniqueRef<SceneSystem>> systems;

		for (const auto& func : GetMap())
		{
			systems.emplace_back(func(scene));
		}

		std::sort(systems.begin(), systems.end(),
			[](const UniqueRef<SceneSystem>& a, const UniqueRef<SceneSystem>& b)
			{
				return a->GetSetupPriority() < b->GetSetupPriority();
			});

		return systems;
	}

	std::vector<SceneSystemFactory::SceneSystemCreateFunc>& SceneSystemFactory::GetMap()
	{
		static std::vector<SceneSystemFactory::SceneSystemCreateFunc> _sCreateFunctions;
		return _sCreateFunctions;
	}
}