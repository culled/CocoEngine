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

		/// @brief Gets the type of resource for the associated file extension
		/// @param extension The extension (including the ".")
		/// @return The resource type
		virtual const std::type_index GetResourceTypeForExtension(const string& extension) const = 0;

		/// @brief Serializes the given resource into binary data
		/// @param resource The resource
		/// @return The serialized data
		virtual string Serialize(SharedRef<Resource> resource) = 0;

		/// @brief Deserializes a resource from the given data
		/// @param type The type of resource to deserialize
		/// @param resourceID The id for the resource
		/// @param data The serialized resource data
		/// @return A resource
		virtual SharedRef<Resource> Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data) = 0;
	};
}