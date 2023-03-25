#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "SceneEntity.h"

namespace Coco
{
	class COCOAPI Scene
	{
	private:
		List<Ref<SceneEntity>> _entities;

	public:
		void AddEntity(Ref<SceneEntity> entity);
		
		const List<Ref<SceneEntity>>& GetEntities() const noexcept { return _entities; }

		template<typename ComponentType>
		List<Ref<SceneEntity>> GetEntitiesWithComponent() const
		{
			List<Ref<SceneEntity>> entities;

			for (const Ref<SceneEntity>& entity : _entities)
			{
				if (entity->GetComponent<ComponentType>() != nullptr)
					entities.Add(entity);
			}

			return entities;
		}
	};
}