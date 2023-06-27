#pragma once

#include "../API.h"
#include "List.h"
#include "Array.h"
#include "../Math/Math.h"
#include <iterator>
#include <cstddef>

namespace Coco
{
	template<typename ValueType>
	struct PackedSetData
	{
		uint64_t SparseIndex = Math::MaxValue<uint64_t>();
		ValueType Data = {};

		PackedSetData() = default;
		PackedSetData(uint64_t index, ValueType&& data) : SparseIndex(index), Data(std::move(data)) {}

		PackedSetData(const PackedSetData&) = delete;
		PackedSetData& operator=(const PackedSetData&) = delete;

		PackedSetData(PackedSetData&& other)
		{
			SparseIndex = other.SparseIndex;
			Data = std::move(other.Data);
		}

		PackedSetData& operator=(PackedSetData&& other)
		{
			SparseIndex = other.SparseIndex;
			Data = std::move(other.Data);

			return *this;
		}
	};

	template<typename ValueType, uint64_t Size>
	class COCOAPI SparseSet
	{
	public:
		struct Iterator
		{
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type = uint64_t;
			using value_type = ValueType;
			using pointer = ValueType*;
			using reference = ValueType&;

		private:
			PackedSetData<ValueType>* _ptr;
			uint64_t _offset;

		public:
			Iterator(PackedSetData<ValueType>* ptr, uint64_t offset) : _ptr(ptr), _offset(offset) {}

			constexpr reference operator*() const { return (*(_ptr + _offset)).Data; }
			constexpr pointer operator->() const { return &((_ptr + _offset)->Data); }

			constexpr Iterator& operator++() { _offset++; return *this; }
			constexpr Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

			constexpr Iterator& operator+=(int delta) { _offset += delta; return *this; }
			constexpr Iterator operator+(int delta) { Iterator tmp = *this; tmp += delta; return tmp; }

			constexpr Iterator& operator--() { _offset--; return *this; }
			constexpr Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

			constexpr Iterator& operator-=(int delta) { _offset -= delta; return *this; }
			constexpr Iterator operator-(int delta) { Iterator tmp = *this; tmp -= delta; return tmp; }

			constexpr reference operator[](int index) { return *(_ptr + index); }

			friend constexpr bool operator==(const Iterator& a, const Iterator& b) { return a._ptr == b._ptr && a._offset == b._offset; }
			friend constexpr bool operator!=(const Iterator& a, const Iterator& b) { return a._ptr != b._ptr || a._offset != b._offset; }

			friend constexpr bool operator<(const Iterator& a, const Iterator& b) { return a._ptr == b._ptr && a._offset < b._offset; }
			friend constexpr bool operator<=(const Iterator& a, const Iterator& b) { return a._ptr == b._ptr && a._offset <= b._offset; }

			friend constexpr bool operator>(const Iterator& a, const Iterator& b) { return a._ptr == b._ptr && a._offset > b._offset; }
			friend constexpr bool operator>=(const Iterator& a, const Iterator& b) { return a._ptr == b._ptr && a._offset >= b._offset; }

			friend constexpr difference_type operator-(const Iterator& a, const Iterator& b) { return a._offset - b._offset; }
		};

		using PackedIterator = List<PackedSetData<ValueType>>::Iterator;

	public:
		static constexpr uint64_t InvalidIndex = Math::MaxValue<uint64_t>();

	private:
		List<PackedSetData<ValueType>> _data;
		Array<uint64_t, Size> _sparseSet;

	public:
		SparseSet()
		{
			for (uint64_t i = 0; i < Size; i++)
				_sparseSet[i] = InvalidIndex;
		}

		SparseSet(uint64_t capacity) : SparseSet(), _data(capacity)
		{
		}

		ValueType& Add(uint64_t index, ValueType&& value)
		{
			_data.Add(PackedSetData<ValueType>(index, std::forward<ValueType>(value)));
			_sparseSet[index] = _data.Count() - 1;

			return _data.Last().Data;
		}

		bool Contains(uint64_t index) const
		{
			return _sparseSet[index] != InvalidIndex;
		}

		bool TryGet(uint64_t index, ValueType*& value)
		{
			for (auto& data : _data)
			{
				if (data.SparseIndex == index)
				{
					value = &data.Data;
					return true;
				}
			}

			return false;
		}

		ValueType& Get(uint64_t index)
		{
			return _data[_sparseSet[index]].Data;
		}

		bool Remove(uint64_t index)
		{
			PackedIterator it = _data.Find([index](const PackedSetData<ValueType>& value) { return value.SparseIndex == index; });

			if (it == _data.end())
				return false;

			// Move the end element's data to the deleted element's data
			PackedIterator endElementIt = _data.end() - 1;
			it->Data = std::move(endElementIt->Data);

			// Update the index in the sparse set to point to the moved element
			_sparseSet[endElementIt->SparseIndex] = std::distance(_data.begin(), it);
			_sparseSet[index] = InvalidIndex;

			_data.Remove(endElementIt);

			return true;
		}

		uint64_t GetNextIndex() const
		{
			const auto it = std::find(_sparseSet.cbegin(), _sparseSet.cend(), InvalidIndex);

			if (it == _sparseSet.cend())
				return InvalidIndex;

			return std::distance(_sparseSet.cbegin(), it);
		}

		List<PackedSetData<ValueType>>& Data() { return _data; }

		Iterator begin() { return Iterator(_data.Data(), 0); }
		Iterator end() { return Iterator(_data.Data(), _data.Count()); }

		Iterator rbegin() { return Iterator(_data.Data(), _data.Count() - 1); }
		Iterator rend() { return Iterator(_data.Data(), -1); }
	};
}