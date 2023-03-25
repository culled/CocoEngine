#include "Scene.h"

namespace Coco
{
	void Scene::AddEntity(Ref<SceneEntity> entity)
	{
		_entities.Add(entity);
	}
}
