#pragma once

#include <Coco/Core/API.h>

#include <typeinfo>
#include <typeindex>
#include <atomic>

namespace Coco
{
	/// @brief Shared control block for all Refs
	class RefControlBlock
	{
		std::atomic_uint64_t _userCount;
		std::type_index _resourceType;

	public:
		RefControlBlock(const std::type_info& type) noexcept;

		RefControlBlock(const RefControlBlock&) = delete;
		RefControlBlock& operator=(const RefControlBlock&) = delete;

		RefControlBlock(RefControlBlock&& other) noexcept;
		RefControlBlock& operator=(RefControlBlock&& other) noexcept;

		~RefControlBlock() = default;

		/// @brief Gets if the resource is still valid
		/// @return True if the resource is still valid
		bool IsValid() const noexcept { return _resourceType != typeid(std::nullptr_t); }

		/// @brief Gets the resource statically-casted to the given type
		/// @tparam ResourceType The type to cast the resource to
		/// @return A pointer to the resource
		const std::type_index& GetResourceType() const noexcept { return _resourceType; }

		void SetResourceType(const std::type_info& type) { _resourceType = type; }

		/// @brief Adds a user to this resource
		void AddUse() noexcept { _userCount++; }

		/// @brief Removes a user from this resource
		void RemoveUse() noexcept { _userCount--; }

		/// @brief Gets the current number of users
		/// @return The number of users
		uint64_t GetUseCount() const noexcept { return _userCount; }
	};
}