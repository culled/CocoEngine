#pragma once

#include <Coco/Core/Defines.h>
#include "../Entity.h"
#include <yaml-cpp/yaml.h>
#include <Coco/Core/Types/YAMLConverters.h>

namespace Coco::ECS
{
	class BaseComponentSerializer
	{
	public:
		virtual ~BaseComponentSerializer() = default;

		virtual bool ShouldUseForEntity(const Entity& entity) const = 0;
		virtual void Serialize(YAML::Emitter& emitter, const Entity& entity) = 0;
		virtual bool Deserialize(const YAML::Node& baseNode, Entity& entity) = 0;
	};

	template<typename ComponentType>
	class ComponentSerializer :
		public BaseComponentSerializer
	{
	public:
		virtual ~ComponentSerializer() = default;

		bool ShouldUseForEntity(const Entity& entity) const override
		{
			return entity.HasComponent<ComponentType>();
		}
	};
}