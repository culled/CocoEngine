#pragma once

#include "../Types/Refs.h"
#include "../Types/String.h"
#include "Resource.h"
#include "ResourceSerializerFactory.h"
#include <yaml-cpp/yaml.h>

namespace Coco
{
	/// @brief Base class for all resource serializers
	class ResourceSerializer
	{
	public:
		virtual ~ResourceSerializer() = default;

		virtual bool SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource) = 0;
		virtual SharedRef<Resource> CreateResource(const ResourceID& id) = 0;
		virtual bool DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource) = 0;
	};
}