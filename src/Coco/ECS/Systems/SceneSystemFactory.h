#pragma once

#include <Coco/Core/Types/Refs.h>

namespace Coco::ECS
{
	class SceneSystem;
	class Scene;

	class SceneSystemFactory
	{
		friend class SceneSystem;

	public:
		using SceneSystemCreateFunc = std::function<UniqueRef<SceneSystem>(Scene&)>;

	public:
		static std::vector<UniqueRef<SceneSystem>> CreateSystems(Scene& scene);

		template<class SystemType>
		static void Register()
		{
			GetMap().emplace_back(Instatiate<SystemType>);
		}

	private:
		static std::vector<SceneSystemCreateFunc>& GetMap();

	private:
		SceneSystemFactory() = delete;

		template<class SystemType>
		static UniqueRef<SystemType> Instatiate(Scene& scene)
		{
			return CreateUniqueRef<SystemType>(scene);
		}
	};
}