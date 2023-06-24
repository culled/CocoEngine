#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"
#include "Entity.h"
#include "ECSService.h"

#include <iterator>
#include <cstddef>

namespace Coco::ECS
{
	class Scene;

	template<typename ... ComponentTypes>
	class SceneView
	{
	private:
		Scene* _scene;

	public:
		struct Iterator
		{
		private:
			Scene* _scene;
			uint64_t _entityIndex;

		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = uint64_t;

			Iterator(Scene* scene) : _scene(scene), _entityIndex(0)
			{
				const auto& entityData = ECSService::Get()->GetEntities();

				while (_entityIndex < entityData.Count() && !IsValidIndex())
				{
					_entityIndex++;
				}
			}

			Iterator(Scene* scene, uint64_t _entityIndex) : _scene(scene), _entityIndex(_entityIndex)
			{}

			EntityID operator*() const { return GetEntityID(); }

			constexpr bool operator==(const Iterator& other) const { return _scene == other._scene && _entityIndex == other._entityIndex; }
			constexpr bool operator!=(const Iterator& other) const { return _scene != other._scene || _entityIndex != other._entityIndex; }

			Iterator& operator++()
			{
				const auto& entityData = ECSService::Get()->GetEntities();
				
				do
				{
					_entityIndex++;
				} while (_entityIndex < entityData.Count() && !IsValidIndex());

				return *this;
			}

		private:
			bool IsValidIndex()
			{
				ECSService* ecs = ECSService::Get();
				EntityID id = GetEntityID();

				return ecs->IsEntityInScene(id, _scene->GetID()) && ecs->HasComponents<ComponentTypes...>(id);
			}

			EntityID GetEntityID() const
			{
				return ECSService::Get()->GetEntities()[_entityIndex].SparseIndex;
			}
		};

		SceneView(Scene& scene) : _scene(&scene) {}

		Iterator begin() { return Iterator(_scene); }
		Iterator end() { return Iterator(_scene, ECSService::Get()->GetEntities().Count()); }
	};
}