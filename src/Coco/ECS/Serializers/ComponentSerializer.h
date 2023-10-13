#pragma once

#include <Coco/Core/Defines.h>
#include "../Entity.h"

namespace YAML
{
	class Emitter;
	class Node;
}

namespace Coco::ECS
{
	struct ComponentSerializer
	{
		virtual ~ComponentSerializer() = default;
		void Serialize(YAML::Emitter& emitter, const Entity& entity);
		bool Deserialize(const YAML::Node& baseNode, Entity& entity);

	protected:
		virtual const char* GetSectionName() const = 0;
		virtual void SerializeImpl(YAML::Emitter& emitter, const Entity& entity) = 0;
		virtual void DeserializeImpl(const YAML::Node& baseNode, Entity& entity) = 0;
	};
}