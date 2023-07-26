#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Vendor/uuid_v4/uuid_v4.h>
#include <xhash>

namespace Coco
{
	/// @brief A 128-bit unique identifier
	class UUID
	{
	public:
		/// @brief A UUID with all bits set to 0
		static const UUID Nil;

		/// @brief A UUID with all bits set to 1
		static const UUID Max;

	private:
		static UUIDv4::UUIDGenerator<std::mt19937_64> _generator;

		UUIDv4::UUID _uuid;

	public:
		UUID();
		UUID(const UUID& other);
		UUID(uint64_t a, uint64_t b);
		UUID(const string& str);

		/// @brief Creates a random UUID
		/// @return A UUID
		static UUID CreateV4();

		/// @brief Combines multiple UUIDs into a single UUID
		/// @tparam ...Args Type of arguments
		/// @param a The first UUID
		/// @param b The second UUID
		/// @param ...values The remaining UUIDs
		/// @return A combined UUID
		template<typename ... Args>
		static UUID Combine(const UUID& a, const UUID& b, Args&&... values)
		{
			// TODO: should this be improved?
			UUID combined(a.ToHash(), b.ToHash());
			return Combine(combined, values...);
		}

		/// @brief Gets the pretty string representation of this UUID
		/// @return The pretty string representation of this UUID
		string AsString() const { return _uuid.str(); }

		/// @brief Gets the hash of this UUID
		/// @return The hash of this UUID
		uint64_t ToHash() const { return _uuid.hash(); }

		bool operator==(const UUID& other) const { return _uuid == other._uuid; }
		bool operator!=(const UUID& other) const { return _uuid != other._uuid; }

		bool operator<(const UUID& other) const { return _uuid < other._uuid; }
		bool operator<=(const UUID& other) const { return _uuid <= other._uuid; }
		bool operator>(const UUID& other) const { return _uuid > other._uuid; }
		bool operator>=(const UUID& other) const { return _uuid >= other._uuid; }

	private:
		UUID(const UUIDv4::UUID& uuid);

		/// @brief Overload for a single UUID
		/// @param a The UUID
		/// @return The given UUID
		static UUID Combine(const UUID& a)
		{
			return a;
		}
	};
}

namespace std
{
	/// @brief Hashing overload for UUIDs
	template<>
	struct hash<Coco::UUID>
	{
		size_t operator()(const Coco::UUID& uuid) const
		{
			return uuid.ToHash();
		}
	};
}