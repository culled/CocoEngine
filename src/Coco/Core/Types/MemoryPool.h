#pragma once

#include "../API.h"
#include "SparseSet.h"

namespace Coco
{
	/// @brief A pool that can manage objects
	/// @tparam ValueType The type of object that this pool manages
	template<typename ValueType, uint64_t PoolSize>
	class MappedMemoryPool
	{
	public:
		using Iterator = SparseSet<ValueType, PoolSize>::Iterator;

	private:
		SparseSet<ValueType, PoolSize> _set;

	public:
		/// @brief Tries to create an object in the pool
		/// @tparam ...Args The types of arguments to pass to the object's constructor
		/// @param index Will be set to the index of the object in the pool
		/// @param ...args The arguments to pass to the object's constructor
		/// @return True if the object was created
		template<typename ... Args>
		bool TryReserve(uint64_t& index, Args&& ... args)
		{
			auto nextIndex = _set.GetNextIndex();

			if (nextIndex == SparseSet<ValueType, PoolSize>::InvalidIndex)
				return false;

			_set.Add(nextIndex, ValueType(std::forward<Args>(args)...));
			index = nextIndex;

			return true;
		}

		/// @brief Tries to get an object from the pool
		/// @param index The index of the object
		/// @param value Will be set to a pointer to the object if the object is found
		/// @return True if the object was found
		bool TryGet(uint64_t index, ValueType*& value)
		{
			return _set.TryGet(index, value);
		}

		/// @brief Gets an object from the pool. NOTE: make sure the object exists before using this
		/// @param index The index of the object
		/// @return The object
		ValueType& Get(uint64_t index)
		{
			return _set.Get(index);
		}

		/// @brief Releases an object from the pool
		/// @param index The index of the object 
		/// @return True if the object was released
		bool Release(uint64_t index)
		{
			return _set.Remove(index);
		}

		/// @brief Gets a list of pointers to all objects in the pool
		/// @return A list of pointers to this pool's objects
		List<ValueType*> GetObjects() { return _set.GetItems(); }

		Iterator begin() { return _set.begin(); }
		Iterator end() { return _set.end(); }
		Iterator rbegin() { return _set.rbegin(); }
		Iterator rend() { return _set.rend(); }
	};
}