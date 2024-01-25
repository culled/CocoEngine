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
		_scene(scene),
		_isSelfActive(true)
	{}

	void EntityInfoComponent::SetName(const string& name)
	{
		_name = name;
	}

	void EntityInfoComponent::SetSelfActive(bool isActive)
	{
		_isSelfActive = isActive;
	}

	void EntityInfoComponent::SetScene(SharedRef<Scene> scene)
	{
		_scene = scene;
	}
}