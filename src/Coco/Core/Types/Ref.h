#pragma once

#include <Coco/Core/API.h>

#include <memory>

namespace Coco
{
	/// @brief Class that manages the lifetime of an object and can be shared
	/// @tparam ValueType 
	template<class ValueType>
	using Ref = std::shared_ptr<ValueType>;
	
	/// @brief Creates a managed object that can be shared
	/// @tparam ValueType 
	/// @tparam ...Args 
	/// @param ...args Arguments to pass to the object constructor
	/// @return A managed object that can be shared
	template<class ValueType, typename ... Args>
	Ref<ValueType> COCOAPI CreateRef(Args&& ... args)
	{
		return std::make_shared<ValueType>(std::forward<Args>(args)...);
	}

	/// @brief A weak reference to a reference
	/// @tparam ValueType 
	template<class ValueType>
	using WeakRef = std::weak_ptr<ValueType>;
	
	/// @brief Class that manages the lifetime of an object
	/// @tparam ValueType 
	/// @tparam U 
	template<class ValueType, typename ValueDeleter = std::default_delete<ValueType>>
	using Managed = std::unique_ptr<ValueType, ValueDeleter>;

	/// @brief Creates a managed object
	/// @tparam ValueType 
	/// @tparam ...Args 
	/// @param ...args Arguments to pass to the object constructor
	/// @return A managed object
	template<class ValueType, typename ... Args>
	Managed<ValueType> COCOAPI CreateManaged(Args&& ... args)
	{
		return std::make_unique<ValueType>(std::forward<Args>(args)...);
	}

	/// @brief Creates a managed object with a custom deleter
	/// @tparam ValueType 
	/// @tparam ...Args 
	/// @tparam U 
	/// @param ...args Arguments to pass to the object constructor
	/// @return A managed object
	template<class ValueType, typename U, typename ... Args>
	Managed<ValueType, U> COCOAPI CreateManaged(Args&& ... args)
	{
		return std::make_unique<ValueType, U>(std::forward<Args>(args)...);
	}
}