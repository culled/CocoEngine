#pragma once

#include <Coco/Core/Core.h>

#include "Resource.h"
#include <type_traits>

#define DeclareSerializerResourceType std::type_index GetResourceType() const;
#define DefineSerializerResourceType(Type) std::type_index GetResourceType() const { return typeid(Type); }

namespace Coco
{
	class ResourceLibrary;

	class COCOAPI ResourceSerializer
	{
	protected:
		ResourceLibrary* _library;

	public:
		ResourceSerializer(ResourceLibrary* library);
		virtual ~ResourceSerializer() = default;

		virtual std::type_index GetResourceType() const = 0;
		virtual string Serialize(const Ref<Resource>& resource) = 0;
		virtual void Deserialize(const string& data, Resource* resource) = 0;
	};
}