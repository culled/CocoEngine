#pragma once

#include <Coco/Core/Core.h>

#include "SceneTypes.h"
#include <Coco/Rendering/Providers/ISceneDataProvider.h>

namespace Coco::ECS
{
	/// @brief A collection of entities
	class Scene : public Rendering::ISceneDataProvider
	{
		friend class ECSService;

	private:
		SceneID _id;
		string _name;
		SceneID _parentID;

	public:
		Scene(SceneID id, string name, SceneID parentID = RootSceneID);

		/// @brief Gets this scene's ID
		/// @return This scene's ID
		SceneID GetID() const { return _id; }

		/// @brief Sets the name of this scene
		/// @param name The name for this scene
		void SetName(const string& name) { _name = name; }

		/// @brief Get's this scene's name
		/// @return This scene's name
		const string& GetName() const { return _name; }

		/// @brief Sets the parent of this scene
		/// @param parentID The ID of this scene's new parent
		void SetParent(const SceneID& parentID) { _parentID = parentID; }

		/// @brief Gets this scene's parent
		/// @return The ID of this scene's parent scene
		SceneID GetParentID() const { return _parentID; }

		/// @brief Gets data for rendering this scene
		/// @param renderView The view to use for rendering
		virtual void GetSceneData(Ref<Rendering::RenderView> renderView) final;

	protected:
		/// @brief Ticks this scene
		/// @param deltaTime The time since the last tick
		void Tick(double deltaTime);
	};
}