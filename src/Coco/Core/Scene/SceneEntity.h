#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>

#include "Components/EntityComponent.h"

#include <typeinfo>
namespace Coco
{
	class COCOAPI SceneEntity
	{
	private:
		// TEMPORARY
		List<Managed<EntityComponent>> _components;

	public:
		template<typename ComponentType, typename ... Args>
		ComponentType* AddComponent(Args&& ... args)
		{
			_components.Add(CreateManaged<ComponentType>(this, std::forward<Args>(args)...));
			return static_cast<ComponentType*>(_components.Last().get());
		}

		template<typename ComponentType>
		ComponentType* GetComponent() const
		{
			const auto it = _components.Find([](const Managed<EntityComponent>& comp)
			{ 
				return dynamic_cast<ComponentType*>(comp.get()) != nullptr;
			});

			if (it != _components.cend())
				return static_cast<ComponentType*>((*it).get());

			return nullptr;
		}

		List<EntityComponent*> GetComponents() const;
	};
}