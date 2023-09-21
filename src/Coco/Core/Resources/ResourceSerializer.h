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
		virtual std::vector<uint8> Serialize(Ref<Resource> resource) = 0;

		/// @brief Deserializes a resource from the given data
		/// @param resourceID The id for the resource
		/// @param data The serialized resource data
		/// @return A resource
		virtual ManagedRef<Resource> Deserialize(const ResourceID& resourceID, std::span<const uint8> data) = 0;
	};
}