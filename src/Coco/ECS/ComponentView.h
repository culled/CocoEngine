#pragma once

#include "Entity.h"
#include "ECSService.h"
#include "Components/EntityInfoComponent.h"
#include "entt.h"

namespace Coco::ECS
{
	/// @brief A view of entities with given components in a scene
	/// @tparam BaseType The type of component
	/// @tparam ...OtherTypes The other types of components
	template<typename BaseType, typename ... OtherTypes>
	class ComponentView
	{
	public:
		using ViewType = entt::basic_view<entt::get_t<entt::storage_for_t<BaseType>, entt::storage_for_t<OtherTypes>...>, entt::exclude_t<>>;

		class Iterator
		{
		public:
			using value_type = Entity;
			using pointer = Entity*;
			using reference = Entity&;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::forward_iterator_tag;

		public:
			Iterator(ComponentView& view, ViewType::iterator it) :
				_view(view),
				_it(it)
			{
				if (_it != view._view.end())
					_currentEntity = Entity(*_it);

				Update();
			}

			~Iterator() = default;

			Iterator& operator++()
			{
				++_it;
				Update();
				return *this;
			}

			Iterator operator++(int)
			{
				Iterator copy = *this;
				++_it;
				Update();
				return copy;
			}

			reference operator*()
			{
				return _currentEntity;
			}

			const reference operator*() const
			{
				return _currentEntity;
			}

			pointer operator->()
			{
				return &_currentEntity;
			}

			const pointer operator->() const
			{
				return &_currentEntity;
			}

			bool operator==(const Iterator& other) const
			{
				return _currentEntity == other._currentEntity;
			}

			bool operator!=(const Iterator& other) const
			{
				return !(*this == other);
			}

		private:
			ComponentView& _view;
			ViewType::iterator _it;
			Entity _currentEntity;

		private:
			/// @brief Updates the current entity
			void Update()
			{
				while (_it != _view._view.end() && !IsValid())
				{
					_currentEntity = Entity(*_it);
					++_it;
				}

				if (_it == _view._view.end())
					_currentEntity = Entity::Null;
			}

			bool IsValid()
			{
				if (_currentEntity == Entity::Null)
					return false;

				const EntityInfoComponent& info = _currentEntity.GetComponent<EntityInfoComponent>();
				return info.GetScene() == _view._scene;
			}
		};

	public:
		ComponentView(const SharedRef<Scene>& scene) :
			_scene(scene),
			_view(ECSService::Get()->_registry.view<BaseType, OtherTypes...>())
		{}

		~ComponentView() = default;

		Iterator begin() { return Iterator(*this, _view.begin()); }
		Iterator end() { return Iterator(*this, _view.end()); }

	private:
		SharedRef<Scene> _scene;
		ViewType _view;
	};
}