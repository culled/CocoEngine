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

		RefControlBlock(RefControlBlock&& other) noexcept;
		RefControlBlock& operator=(RefControlBlock&& other) noexcept;

		~RefControlBlock() = default;

		RefControlBlock(const RefControlBlock&) = delete;
		RefControlBlock& operator=(const RefControlBlock&) = delete;

		/// @brief Gets if the resource is valid
		/// @return True if the resource is valid
		bool IsValid() const noexcept { return _resourceType != typeid(std::nullptr_t); }

		/// @brief Gets the type of the resource that is managed
		/// @return The type of the resource that is managed
		const std::type_index& GetResourceType() const noexcept { return _resourceType; }

		/// @brief Sets the type of resource that is managed
		/// @param type The type of resource
		void SetResourceType(const std::type_info& type) noexcept { _resourceType = type; }

		/// @brief Adds a user to this resource
		void AddUse() noexcept { _userCount++; }

		/// @brief Removes a user from this resource
		void RemoveUse() noexcept { _userCount--; }

		/// @brief Gets the current number of users
		/// @return The number of users
		uint64_t GetUseCount() const noexcept { return _userCount; }
	};
}