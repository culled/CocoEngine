#pragma once

#include <Coco/Core/Core.h>
#include "EntityData.h"
#include "ECSService.h"

namespace Coco::ECS
{
	class Entity
	{
	public:
		Entity(EntityData& data);

		EntityID GetID() const { return _data.ID; }

		void SetName(string name) { _data.Name = name; }
		string GetName() const { return _data.Name; }

		void Destroy();

		template<typename ComponentType, typename ... Args>
		ComponentType* AddComponent(Args&& ... args)
		{
			return ECSService::Get()->AddComponent<ComponentType>(GetID(), std::forward(args)...);
		}

		template<class ComponentType>
		bool HasComponent() const
		{
			return ECSService::Get()->HasComponent<ComponentType>(GetID());
		}

		template<typename ComponentType, typename ... Args>
		ComponentType* GetComponent(Args&& ... args)
		{
			return ECSService::Get()->GetComponent<ComponentType>(GetID());
		}

		template<class ComponentType>
		void RemoveComponent()
		{
			ECSService::Get()->RemoveComponent<ComponentType>(GetID());
		}

	private:
		EntityData& _data;

		friend class EntityManager;
	};
}