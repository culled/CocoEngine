#pragma once

#include "../Defines.h"
#include "../Types/Refs.h"
#include "../Types/String.h"
#include "ResourceTypes.h"

namespace Coco
{
	/// @brief Base class for Engine resources
	class Resource :
		public std::enable_shared_from_this<Resource>
	{
		friend class ResourceLibrary;

	public:
		Resource(const ResourceID& id);
		virtual ~Resource() = default;

		/// @brief Gets the type of this resource
		/// @return This resource's type
		virtual const std::type_info& GetType() const = 0;

		/// @brief Gets the typename of this resource
		/// @return This resource's typename
		virtual const char* GetTypename() const = 0;

		/// @brief Gets the ID of this resource
		/// @return This resource's ID
		const ResourceID& GetID() const { return _id; }

		/// @brief Gets the version of this resource
		/// @return This resource's version
		const ResourceVersion& GetVersion() const { return _version; }

	protected:
		/// @brief Sets the version of this resource
		/// @param version The new version
		void SetVersion(const ResourceVersion& version);

		/// @brief Increments the version of this resource
		void IncrementVersion();

		/// @brief Gets a self-reference for this resource
		/// @tparam DerivedType The derived type
		/// @return A self-reference for the derived type
		template<typename DerivedType>
		SharedRef<DerivedType> GetSelfRef()
		{
			return std::static_pointer_cast<DerivedType>(shared_from_this());
		}

	private:
		ResourceID _id;
		ResourceVersion _version;
	};
}