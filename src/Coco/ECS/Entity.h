#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include "EntityTypes.h"
#include "SceneTypes.h"

namespace Coco::ECS
{
	/// @brief Represents an entity in a scene
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

		/// @brief Sets the name of this entity
		/// @param name The new name
		void SetName(string name) { _name = name; }

		/// @brief Gets the name of this entity
		/// @return The name
		const string& GetName() const { return _name; }

		/// @brief Sets the parent of this entity
		/// @param parentID The ID of the parent entity, or InvalidEntityID to clear this entity's parent
		void SetParentID(const EntityID& parentID);

		/// @brief Gets the ID of this entity's parent
		/// @return This ID of this entity's parent
		const EntityID& GetParentID() const { return _parentID; }

		/// @brief Gets this entity's parent, checking first if it exists
		/// @param parent Will be set to this entity's parent
		/// @return True if this entity's parent exists
		bool TryGetParent(Entity*& parent);

		/// @brief Gets the direct child entities of this entity
		/// @return A list of this entity's children
		List<Entity*> GetChildren() const;

		/// @brief Gets the ID of the scene that this entity exists in
		/// @return The scene ID where this entity exists
		SceneID GetSceneID() const { return _sceneID; }
	};
}