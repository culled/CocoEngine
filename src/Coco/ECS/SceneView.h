#pragma once

#include "Entity.h"
#include "entt.h"

namespace Coco::ECS
{
	//template<typename BaseType, typename ... OtherTypes>
	//using ViewType = entt::basic_view<entt::get_t<entt::storage_for_t<BaseType>, entt::storage_for_t<OtherTypes>...>, entt::exclude_t<>>;

	/// @brief An iterator for entities within a scene
	/// @tparam BaseType The type of component
	/// @tparam ...OtherTypes The other types of components
	template<typename BaseType, typename ... OtherTypes>
	class SceneViewIterator
	{
	public:
		using value_type = Entity;
		using pointer = Entity*;
		using reference = Entity&;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

	private:
		using ViewType = entt::basic_view<entt::get_t<entt::storage_for_t<BaseType>, entt::storage_for_t<OtherTypes>...>, entt::exclude_t<>>;

		ViewType::iterator _it;
		SharedRef<Scene> _scene;
		Entity _currentEntity;

	public:
		SceneViewIterator(const SharedRef<Scene>& scene, ViewType::iterator it) :
			_scene(scene),
			_it(it)
		{
			Update();
		}

		~SceneViewIterator() = default;

		SceneViewIterator& operator++()
		{
			++_it;
			Update();
			return *this;
		}

		SceneViewIterator operator++(int)
		{
			SceneViewIterator copy = *this;
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

		bool operator==(const SceneViewIterator& other) const
		{
			return _currentEntity == other._currentEntity;
		}

		bool operator!=(const SceneViewIterator& other) const
		{
			return !(*this == other);
		}

	private:
		/// @brief Updates the current entity
		void Update()
		{
			_currentEntity = Entity(*_it, _scene);
		}
	};

	/// @brief A view of entities with given components in a scene
	/// @tparam BaseType The type of component
	/// @tparam ...OtherTypes The other types of components
	template<typename BaseType, typename ... OtherTypes>
	class SceneView
	{
		friend class Scene;

	private:
		using ViewType = entt::basic_view<entt::get_t<entt::storage_for_t<BaseType>, entt::storage_for_t<OtherTypes>...>, entt::exclude_t<>>;

		SharedRef<Scene> _scene;
		ViewType _view;

	public:
		SceneView(const SharedRef<Scene>& scene) :
			_scene(scene),
			_view(scene->_registry.view<BaseType, OtherTypes...>())
		{}

		~SceneView() = default;

		SceneViewIterator<BaseType, OtherTypes...> begin() { return SceneViewIterator<BaseType, OtherTypes...>(_scene, _view.begin()); }
		SceneViewIterator<BaseType, OtherTypes...> end() { return SceneViewIterator<BaseType, OtherTypes...>(_scene, _view.end()); }
	};
}