#pragma once

#include <Coco/Core/Core.h>

#include "SceneTypes.h"
#include <Coco/Rendering/Providers/ISceneDataProvider.h>

namespace Coco::ECS
{
	class Scene : public Rendering::ISceneDataProvider
	{
	private:
		SceneID _id;
		string _name;
		SceneID _parentID;

		friend class ECSService;

	public:
		Scene(SceneID id, string name, SceneID parentID = RootSceneID);

		SceneID GetID() const { return _id; }

		void SetName(const string& name) { _name = name; }
		const string& GetName() const { return _name; }

		void SetParent(SceneID parentID) { _parentID = parentID; }
		SceneID GetParentID() const { return _parentID; }

		virtual void GetSceneData(Ref<Rendering::RenderView> renderView) final;

	protected:
		void Tick(double deltaTime);
	};
}