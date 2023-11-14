#pragma once

#include "../Entity.h"
#include <Coco/Core/MainLoop/TickInfo.h>

namespace Coco::ECS
{
	/// @brief Base class for scrips that can be attached to a NativeScriptComponent
	class NativeScript
	{
		friend class NativeScriptSystem;
		friend class NativeScriptComponent;

	private:
		Entity _entity;

	public:
		virtual ~NativeScript() = default;

		/// @brief Gets the entity that this script is attached to
		/// @return The entity that this script is attached to
		const Entity& GetEntity() const { return _entity; }

		/// @brief Adds a component to the entity this script is attached to
		/// @tparam ComponentType The type of component to add
		/// @tparam ...Args The type of constructor arguments
		/// @param ...args The arguments to pass to the component's constructor
		/// @return The added component
		template<typename ComponentType, typename ... Args>
		ComponentType& AddComponent(Args&& ... args) { return _entity.AddComponent<ComponentType>(std::forward<Args>(args)...); }

		/// @brief Gets a component on the entity this script is attached to
		/// @tparam ComponentType The type of component to get
		/// @return The component
		template<typename ComponentType>
		ComponentType& GetComponent() const { return _entity.GetComponent<ComponentType>(); }

		/// @brief Determines if the entity this script is attached to has a component of the given type
		/// @tparam ComponentType The type of component
		/// @return True if the entity has a component of the given type
		template<typename ComponentType>
		bool HasComponent() const { return _entity.HasComponent<ComponentType>(); }

		/// @brief Gets a type of component if the entity this script is attached to has it
		/// @tparam ComponentType The type of component to get
		/// @param outComponent Will be set to the component if found
		/// @return True if the component was found
		template<typename ComponentType>
		bool TryGetComponent(ComponentType*& outComponent) { return _entity.TryGetComponent<ComponentType>(outComponent); }

	protected:
		/// @brief Called after this script has been setup
		virtual void OnCreate() {}

		/// @brief Called on the first tick that this script is active
		virtual void OnStart() {}

		/// @brief Called every tick
		/// @param tickInfo The info of the current tick
		virtual void OnUpdate(const TickInfo& tickInfo) {}

		/// @brief Called when this script will be destroyed
		virtual void OnDestroy() {}
	};
}