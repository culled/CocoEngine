#include "ECSpch.h"
#include "EntityInfoComponent.h"

namespace Coco::ECS
{
	EntityInfoComponent::EntityInfoComponent(const Entity& owner) :
		EntityInfoComponent(owner, "Entity", nullptr)
	{}

	EntityInfoComponent::EntityInfoComponent(const Entity& owner, const string& name, const SharedRef<Scene>& scene) :
		EntityComponent(owner),
		_name(name),
		_scene(scene)
	{}

	void EntityInfoComponent::SetName(const string& name)
	{
		_name = name;
	}

	void EntityInfoComponent::SetScene(SharedRef<Scene> scene)
	{
		_scene = scene;
	}
}