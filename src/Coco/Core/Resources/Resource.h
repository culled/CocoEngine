#pragma once

#include "../Defines.h"
#include "../Types/String.h"
#include "../Math/Math.h"

namespace Coco
{
	/// @brief An ID for a resource
	using ResourceID = uint64;

	/// @brief A version for a resource
	using ResourceVersion = uint64;

	/// @brief Base class for Engine resources
	class Resource
	{
		friend class ResourceLibrary;

	public:
		/// @brief An invalid resource ID
		static constexpr ResourceID InvalidID = Math::MaxValue<ResourceID>();

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

	protected:
		/// @brief Sets the version of this resource
		/// @param version The new version
		void SetVersion(const ResourceVersion& version);
	};
}