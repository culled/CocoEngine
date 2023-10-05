#include "ECSpch.h"
#include "Entity.h"

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

	bool Entity::operator==(const Entity & other) const
	{
		if (_handle != other._handle)
			return false;

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

	uint64 Entity::GetID() const
	{
		return static_cast<uint64>(_handle);
	}

	SharedRef<Scene> Entity::GetScene() const
	{
		return _scene.expired() ? nullptr : _scene.lock();
	}
}