#pragma once

#include "../API.h"
#include "SparseSet.h"

namespace Coco
{
	template<typename ValueType, uint64_t PoolSize>
	class MappedMemoryPool
	{
	public:
		using Iterator = SparseSet<ValueType, PoolSize>::Iterator;

	private:
		SparseSet<ValueType, PoolSize> _set;

	public:
		bool TryReserve(ValueType&& value, uint64_t& index)
		{
			auto nextIndex = _set.GetNextIndex();

			if (nextIndex == SparseSet<ValueType, PoolSize>::InvalidIndex)
				return false;

			_set.Add(nextIndex, std::forward<ValueType>(value));
			index = nextIndex;

			return true;
		}

		uint64_t GetNextReserveIndex() const
		{
			return _set.GetNextIndex();
		}

		bool TryGet(uint64_t index, ValueType*& value)
		{
			return _set.TryGet(index, value);
		}

		ValueType& Get(uint64_t index)
		{
			return _set.Get(index);
		}

		bool Release(uint64_t index)
		{
			return _set.Remove(index);
		}

		SparseSet<ValueType, PoolSize>& GetSparseSet() { return _set; }

		Iterator begin() { return _set.begin(); }
		Iterator end() { return _set.end(); }
		Iterator rbegin() { return _set.rbegin(); }
		Iterator rend() { return _set.rend(); }
	};
}