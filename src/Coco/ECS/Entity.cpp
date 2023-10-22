#include "ECSpch.h"
#include "Entity.h"

#include "Components/EntityInfoComponent.h"

#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	const Entity Entity::Null = Entity();

	Entity::Entity(entt::entity handle, SharedRef<Scene> scene) :
		_handle(handle),
		_scene(scene)
	{}

	Entity::Entity() :
		Entity(entt::null, nullptr)
	{}

	Entity::Entity(const Entity & other) :
		_handle(other._handle),
		_scene(other._scene)
	{}

	bool Entity::operator==(const Entity& other) const
	{
		if (_handle != other._handle)
			return false;

		// Same handle, so check if both are null handles
		if (_handle == entt::null)
			return true;

		// Can't be the same if one's scene has expired
		if (_scene.expired() != other._scene.expired())
			return false;

		if (_scene.expired() && other._scene.expired())
			return true;

		return _scene.lock() == other._scene.lock();
	}

	bool Entity::IsValid() const
	{
		if (_scene.expired())
			return false;

		SharedRef<Scene> s = _scene.lock();
		return s->_registry.valid(_handle);
	}

	EntityID Entity::GetID() const
	{
		return GetComponent<EntityInfoComponent>().ID;
	}

	SharedRef<Scene> Entity::GetScene() const
	{
		return _scene.expired() ? nullptr : _scene.lock();
	}

	void Entity::SetParent(const Entity& parent)
	{
		Assert(IsValid())
		Assert(parent.IsValid())

		SharedRef<Scene> scene = _scene.lock();
		Assert(parent._scene.lock() == scene)

		if (parent.IsDescendentOf(*this))
		{
			CocoError("Cannot parent entity as it would make a circular hierarchy")
			return;
		}

		scene->ReparentEntity(GetID(), parent.GetID());
	}

	bool Entity::HasParent() const
	{
		Assert(IsValid())

		SharedRef<Scene> scene = _scene.lock();
		return scene->_entityParentMap.contains(GetID());
	}

	void Entity::ClearParent()
	{
		Assert(IsValid())

		SharedRef<Scene> scene = _scene.lock();
		scene->ReparentEntity(GetID(), InvalidEntityID);
	}

	bool Entity::IsDescendentOf(const Entity& ancestor) const
	{
		Assert(IsValid())
		Assert(ancestor.IsValid())

		if (ancestor == *this)
			return true;

		if (!HasParent())
			return false;

		return GetParent().IsDescendentOf(ancestor);
	}

	Entity Entity::GetParent() const
	{
		Assert(IsValid())

		SharedRef<Scene> scene = _scene.lock();
		auto it = scene->_entityParentMap.find(GetID());
		
		if (it == scene->_entityParentMap.end())
			return Entity::Null;

		Entity parent;
		Assert(scene->TryGetEntity(it->second, parent))
		return parent;
	}

	std::vector<Entity> Entity::GetChildren() const
	{
		Assert(IsValid())

		std::vector<Entity> children;
		SharedRef<Scene> scene = _scene.lock();
		EntityID id = GetID();

		for (const auto& kvp : scene->_entityParentMap)
		{
			if (kvp.second != id)
				continue;

			Entity& child = children.emplace_back();
			Assert(scene->TryGetEntity(kvp.first, child))
		}

		return children;
	}
}