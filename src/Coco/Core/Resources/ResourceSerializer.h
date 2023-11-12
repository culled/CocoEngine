#pragma once

#include "../Types/Refs.h"
#include "../Types/String.h"
#include "Resource.h"

namespace Coco
{
	/// @brief Base class for all resource serializers
	struct ResourceSerializer
	{
		virtual ~ResourceSerializer() = default;

		/// @brief Determines if this serializer supports serializing files with the given extension
		/// @param extension The extension (including the ".")
		/// @return True if this serializer supports the extension
		virtual bool SupportsFileExtension(const string& extension) const = 0;

		/// @brief Determines if this serializer supports serializing resources of the given type
		/// @param type The type of resource
		/// @return True if this serializer supports the resource type
		virtual bool SupportsResourceType(const std::type_index& type) const = 0;

		/// @brief Serializes the given resource into binary data
		/// @param resource The resource
		/// @return The serialized data
		virtual string Serialize(SharedRef<Resource> resource) = 0;

		/// @brief Creates a resource and deserializes data into it
		/// @param id The resource ID
		/// @param name The resource name
		/// @param data The serialized resource data
		/// @return The deserialized resource
		virtual SharedRef<Resource> CreateAndDeserialize(const ResourceID& id, const string& name, const string& data) = 0;

		/// @brief Deserializes a resource from the given data
		/// @param data The serialized resource data
		/// @param resource The resource to deserialize into
		/// @return True if the resource was deserialized
		virtual bool Deserialize(const string& data, SharedRef<Resource> resource) = 0;
	};
}