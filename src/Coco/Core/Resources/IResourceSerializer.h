#pragma once

#include <Coco/Core/Core.h>

#include "Resource.h"
#include <type_traits>

#define DeclareSerializerResourceType std::type_index GetResourceType() const;
#define DefineSerializerResourceType(Type) std::type_index GetResourceType() const { return typeid(Type); }

namespace Coco
{
	class ResourceLibrary;

	/// @brief An interface for classes that can serialize/deserialize resources
	class COCOAPI IResourceSerializer
	{
	protected:
		IResourceSerializer() = default;

	public:
		virtual ~IResourceSerializer() = default;

		/// @brief Gets the type of resource that this serializer works with
		/// @return The type of resource that this serializer works with
		virtual std::type_index GetResourceType() const = 0;

		/// @brief Serializes a resource to a string
		/// @param library The library of the resource
		/// @param resource The resource to serialize
		/// @return The string representation of the resource
		virtual string Serialize(ResourceLibrary* library, const Ref<Resource>& resource) = 0;

		/// @brief Deserializes data into a resource
		/// @param library The library of the resource
		/// @param data The serialized data
		/// @param resource The resource to deserialize into
		virtual void Deserialize(ResourceLibrary* library, const string& data, Ref<Resource> resource) = 0;
	};
}