#pragma once

#include "../Defines.h"
#include "../Types/String.h"

namespace Coco
{
	/// @brief A version for a resource
	using ResourceVersion = uint64;

	class ResourceID
	{
	public:
		static const ResourceID Invalid;

	public:
		ResourceID();
		ResourceID(const string& idStr);
		ResourceID(const uint64& hash);

		bool operator==(const ResourceID& other) const { return _hash == other._hash; }
		bool operator==(const uint64& other) const { return _hash == other; }
		bool operator==(const string& other) const { return _id == other; }

		operator string() const { return _id; }
		operator uint64() const { return _hash; }

		const string& ToString() const { return _id; }

	private:
		string _id;
		uint64 _hash;
	};
}

namespace std
{
	template<>
	struct hash<Coco::ResourceID>
	{
		size_t operator()(const Coco::ResourceID& id) const noexcept
		{
			return static_cast<uint64>(id);
		}
	};
}
