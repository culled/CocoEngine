#pragma once

#include <Coco/Core/Corepch.h>
#include <Coco/Core/Defines.h>

namespace Coco
{
	/// @brief A wrapper for std::shared_ptr
	/// @tparam ValueType The type of object
	template<typename ValueType>
	using SharedRef = std::shared_ptr<ValueType>;

	/// @brief Wrapper for std::make_shared
	/// @tparam ValueType The type of object
	/// @tparam ...Args The types of arguments
	/// @param ...args The arguments to pass to the object's constructor
	/// @return The shared reference
	template<typename ValueType, typename ... Args>
	SharedRef<ValueType> CreateSharedRef(Args&& ... args)
	{
		return std::make_shared<ValueType>(std::forward<Args>(args)...);
	}

	/// @brief A wrapper for std::unique_ptr
	/// @tparam ValueType The type of object
	/// @tparam DeleterType The type of deleter
	template<typename ValueType, typename DeleterType = std::default_delete<ValueType>>
	using UniqueRef = std::unique_ptr<ValueType, DeleterType>;

	/// @brief Wrapper for std::make_unique
	/// @tparam ValueType The type of object
	/// @tparam ...Args The types of arguments
	/// @param ...args The arguments to pass to the object's constructor
	/// @return The unique reference
	template<typename ValueType, typename ... Args>
	UniqueRef<ValueType> CreateUniqueRef(Args&&... args)
	{
		return std::move(std::make_unique<ValueType>(std::forward<Args>(args)...));
	}

	/// @brief Tries to find a SharedRef in the given range of iterators
	/// @tparam InputIt The type of iterator
	/// @tparam ValueType The type of value the SharedRef holds
	/// @param start The start
	/// @param end The end
	/// @param ref The SharedRef to find
	/// @return An iterator pointing to the SharedRef if found
	template<class InputIt, typename ValueType>
	InputIt FindSharedRef(
		InputIt start,
		InputIt end,
		const SharedRef<ValueType>& ref)
	{
		return std::find_if(start, end, [ref](const SharedRef<ValueType>& other) { return ref.get() == other.get(); });
	}

	/// @brief Tries to find a UniqueRef in the given range of iterators
	/// @tparam InputIt The type of iterator
	/// @tparam ValueType The type of value the UniqueRef holds
	/// @param start The start
	/// @param end The end
	/// @param ref The UniqueRef to find
	/// @return An iterator pointing to the UniqueRef if found
	template<class InputIt, typename ValueType>
	InputIt FindUniqueRef(
		InputIt start,
		InputIt end,
		const UniqueRef<ValueType>& ref)
	{
		return std::find_if(start, end, [ref](const UniqueRef<ValueType>& other) { return ref.get() == other.get(); });
	}
}