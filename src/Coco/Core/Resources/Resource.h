#pragma once

#include "../Defines.h"
#include "../Types/Refs.h"
#include "../Types/String.h"

namespace Coco
{
	/// @brief An ID for a resource
	using ResourceID = uint64;

	/// @brief A version for a resource
	using ResourceVersion = uint64;

	/// @brief Base class for Engine resources
	class Resource :
		public std::enable_shared_from_this<Resource>
	{
		friend class ResourceLibrary;

	public:
		/// @brief An invalid resource ID
		static const ResourceID InvalidID;

	private:
		ResourceID _id;
		ResourceVersion _version;
		string _name;
		string _contentPath;

	public:
		Resource(const ResourceID& id, const string& name);
		virtual ~Resource() = default;

		/// @brief Gets the underlying type of this resource
		/// @return This resource's type
		virtual std::type_index GetType() const = 0;

		/// @brief Gets the ID of this resource
		/// @return This resource's ID
		const ResourceID& GetID() const { return _id; }

		/// @brief Gets the version of this resource
		/// @return This resource's version
		const ResourceVersion& GetVersion() const { return _version; }

		/// @brief Sets the name of this resource
		/// @param name The name
		void SetName(const string& name);

		/// @brief Gets the name of this resource
		/// @return This resource's name
		const string& GetName() const { return _name; }

		/// @brief Gets the content path of this resource, if it is associated with a file
		/// @return This resource's content path
		const string& GetContentPath() const { return _contentPath; }

	protected:
		/// @brief Sets the version of this resource
		/// @param version The new version
		void SetVersion(const ResourceVersion& version);

		/// @brief Gets a self-reference for this resource
		/// @tparam DerivedType The derived type
		/// @return A self-reference for the derived type
		template<typename DerivedType>
		SharedRef<DerivedType> GetSelfRef()
		{
			return std::static_pointer_cast<DerivedType>(shared_from_this());
		}
	};
}