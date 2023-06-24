#pragma once

#include <Coco/Core/Core.h>
#include "EntityTypes.h"
#include "SceneTypes.h"
#include "ECSService.h"

namespace Coco::ECS
{
	class Entity
	{
	private:
		EntityID _id;
		string _name;
		EntityID _parentID;
		SceneID _sceneID;

		friend ECSService;

	public:
		Entity();
		Entity(EntityID id, const string& name, SceneID sceneID, EntityID parentID = InvalidEntityID);

		bool operator==(const Entity& other) const { return _id == other._id; }
		bool operator!=(const Entity& other) const { return _id != other._id; }

		EntityID GetID() const { return _id; }

		void SetName(string name) { _name = name; }
		const string& GetName() const { return _name; }

		void SetParent(const Entity& parent);
		EntityID GetParentID() const { return _parentID; }
		bool TryGetParent(Entity*& parent);

		SceneID GetSceneID() const { return _sceneID; }

		List<Entity*> GetChildren() const;

		template<typename ComponentType, typename ... Args>
		ComponentType& AddComponent(Args&& ... args)
		{
			return ECSService::Get()->AddComponent<ComponentType>(_id, std::forward<Args>(args)...);
		}

		template<typename ComponentType>
		ComponentType& GetComponent()
		{
			return ECSService::Get()->GetComponent<ComponentType>(_id);
		}

		template<typename ComponentType>
		bool HasComponent() const
		{
			return ECSService::Get()->HasComponent<ComponentType>(_id);
		}

		template<typename ComponentType>
		bool RemoveComponent()
		{
			return ECSService::Get()->RemoveComponent<ComponentType>(_id);
		}
	};
}