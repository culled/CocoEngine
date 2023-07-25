#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "EntityTypes.h"
#include "SceneTypes.h"

namespace Coco::ECS
{
	class Entity
	{
		friend class ECSService;

	public:
		/// @brief The id of this entity
		EntityID ID;

	private:
		string _name;
		EntityID _parentID;
		SceneID _sceneID;


	public:
		Entity();
		Entity(const EntityID& id, const string& name, SceneID sceneID, const EntityID& parentID = InvalidEntityID);

		bool operator==(const Entity& other) const { return ID == other.ID; }
		bool operator!=(const Entity& other) const { return ID != other.ID; }

		void SetName(string name) { _name = name; }
		const string& GetName() const { return _name; }

		void SetParentID(const EntityID& parentID);
		const EntityID& GetParentID() const { return _parentID; }
		bool TryGetParent(Entity*& parent);

		SceneID GetSceneID() const { return _sceneID; }

		List<Entity*> GetChildren() const;
	};
}