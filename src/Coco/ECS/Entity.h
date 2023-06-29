#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "EntityTypes.h"
#include "SceneTypes.h"

namespace Coco::ECS
{
	class Entity
	{
	private:
		EntityID _id;
		string _name;
		EntityID _parentID;
		SceneID _sceneID;

		friend class ECSService;

	public:
		Entity();
		Entity(EntityID id, const string& name, SceneID sceneID, EntityID parentID = InvalidEntityID);

		bool operator==(const Entity& other) const { return _id == other._id; }
		bool operator!=(const Entity& other) const { return _id != other._id; }

		EntityID GetID() const { return _id; }

		void SetName(string name) { _name = name; }
		const string& GetName() const { return _name; }

		void SetParentID(const EntityID& parentID);
		EntityID GetParentID() const { return _parentID; }
		bool TryGetParent(Entity*& parent);

		SceneID GetSceneID() const { return _sceneID; }

		List<Entity*> GetChildren() const;
	};
}