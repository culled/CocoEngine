#pragma once

#include <Coco/Core/API.h>
#include <vector>
#include <functional>

namespace Coco
{
	/// @brief A resizable array of items
	/// @tparam ValueType 
	template<typename ValueType>
	class COCOAPI List
	{
	public:
		/// @brief An iterator for the list
		using Iterator = std::vector<ValueType>::iterator;

		/// @brief A constant iterator for the list
		using ConstIterator = std::vector<ValueType>::const_iterator;

		/// @brief An reverse iterator for the list
		using ReverseIterator = std::vector<ValueType>::reverse_iterator;

		/// @brief A reverse constant iterator for the list
		using ReverseConstIterator = std::vector<ValueType>::const_reverse_iterator;

	private:
		std::vector<ValueType> _list;

	public:
		List() noexcept = default;
		virtual ~List() = default;

		List(uint64_t initialSize) : _list(initialSize) {}
		List(std::initializer_list<ValueType>&& items) : _list(items) {}

		/// @brief Adds an item to this list
		/// @param item The item to add
		constexpr void Add(ValueType&& item) { _list.push_back(std::forward<ValueType>(item)); }

		/// @brief Adds an item to this list
		/// @param item The item to add
		constexpr void Add(const ValueType& item) { _list.push_back(item); }

		/// @brief Inserts an item at the specified index
		/// @param index The index that the item will be insterted at
		/// @param item The item to insert
		void Insert(uint64_t index, ValueType&& item)
		{
			if (index > Count())
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} <= {1}", index, Count()));

			_list.insert(begin() + index, std::forward<ValueType>(item));
		}

		/// @brief Inserts an item at the specified index
		/// @param index The index that the item will be insterted at
		/// @param item The item to insert
		void Insert(uint64_t index, const ValueType& item)
		{
			if (index > Count())
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} <= {1}", index, Count()));

			_list.insert(begin() + index, item);
		}

		/// @brief Removes the first occurance of an item from this list
		/// @param item The item to remove
		/// @return True if the item was found and removed
		bool Remove(const ValueType& item) noexcept
		{
			Iterator it = Find(item);

			if (it != end())
				return Remove(it);

			return false;
		}

		/// @brief Remove an item at the given iterator
		/// @param it The iterator
		/// @return True if the item was erased successfully
		bool Remove(Iterator it) noexcept
		{
			if (it == end())
				return false;

			try
			{
				_list.erase(it);
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		/// @brief Erases an item at the given iterator and returns the element after the one erased, or the list end if the element was the last one erased
		/// @param it The iterator
		/// @return An iterator to the element after the one erased, or the list end if the element was the last one erased
		constexpr Iterator EraseAndGetNext(Iterator it) { return _list.erase(it); }

		/// @brief Removes all items from this list
		constexpr void Clear() noexcept { _list.clear(); }

		/// @brief Gets the number of items in this list
		/// @return The number of items in this list
		constexpr uint64_t Count() const noexcept { return _list.size(); }

		/// @brief Gets the first element in the list
		/// @return The first element of the list
		constexpr ValueType& First() noexcept { return _list.front(); }

		/// @brief Gets the first element in the list
		/// @return The first element of the list
		constexpr const ValueType& First() const noexcept { return _list.front(); }

		/// @brief Gets the last element in the list
		/// @return The last element in the list
		constexpr ValueType& Last() noexcept { return _list.back(); }

		/// @brief Gets the last element in the list
		/// @return The last element in the list
		constexpr const ValueType& Last() const noexcept { return _list.back(); }

		/// @brief Resizes this list to be a new size
		/// @param newSize The new size for this list
		constexpr void Resize(uint64_t newSize) { _list.resize(newSize); }

		/// @brief Gets a pointer to this list's first item
		/// @return A pointer to this list's first item
		constexpr const ValueType* Data() const noexcept { return _list.data(); }

		/// @brief Gets a pointer to this list's first item
		/// @return A pointer to this list's first item
		constexpr ValueType* Data() noexcept { return _list.data(); }

		/// @brief Determines if a given element exists in this list
		/// @param item The item to search for
		/// @return True if the item is found in this list
		bool Contains(const ValueType& item) const noexcept
		{
			if (Find(item) != end())
				return true;

			return false;
		}

		/// @brief Determines if a given element exists in this list by using a predicate function
		/// @param predicate The function to test if the item is found
		/// @return True if the item is found in this list
		bool Contains(std::function<bool(const ValueType&)> predicate) const noexcept
		{
			if (Find(predicate) != end())
				return true;

			return false;
		}

		/// @brief Tries to find the given item in this list
		/// @param item The item to find
		/// @return An iterator to the item if found, else the iterator will equal the end iterator
		constexpr Iterator Find(const ValueType& item)
		{
			return std::find(begin(), end(), item);
		}

		/// @brief Tries to find the given item in this list
		/// @param item The item to find
		/// @return An iterator to the item if found, else the iterator will equal the end iterator
		constexpr ConstIterator Find(const ValueType& item) const
		{
			return std::find(cbegin(), cend(), item);
		}

		/// @brief Tries to find the given item in this list using a predicate function
		/// @param predicate The function to test if the item is found
		/// @return An iterator to the item if found, else the iterator will equal the end iterator
		constexpr Iterator Find(std::function<bool(const ValueType&)> predicate)
		{
			return std::find_if(begin(), end(), predicate);
		}

		/// @brief Tries to find the given item in this list using a predicate function
		/// @param predicate The function to test if the item is found
		/// @return An iterator to the item if found, else the iterator will equal the end iterator
		constexpr ConstIterator Find(std::function<bool(const ValueType&)> predicate) const
		{
			return std::find_if(cbegin(), cend(), predicate);
		}

		/// @brief Gets an iterator for the beginning of this list
		/// @return An iterator for the beginning of this list
		constexpr Iterator begin() noexcept { return _list.begin(); }

		/// @brief Gets an iterator for the beginning of this list
		/// @return An iterator for the beginning of this list
		constexpr ConstIterator begin() const noexcept { return cbegin(); }

		/// @brief Gets an iterator for the beginning of this list
		/// @return An iterator for the beginning of this list
		constexpr ConstIterator cbegin() const noexcept { return _list.cbegin(); }

		/// @brief Gets an iterator for the end of this list
		/// @return An iterator for the end of this list
		constexpr Iterator end() noexcept { return _list.end(); }

		/// @brief Gets an iterator for the end of this list
		/// @return An iterator for the end of this list
		constexpr ConstIterator end() const noexcept { return cend(); }

		/// @brief Gets an iterator for the end of this list
		/// @return An iterator for the end of this list
		constexpr ConstIterator cend() const noexcept { return _list.cend(); }

		/// @brief Gets an iterator for the beginning of this list reversed
		/// @return An iterator for the beginning of this list reversed
		constexpr ReverseIterator rbegin() noexcept { return _list.rbegin(); }

		/// @brief Gets an iterator for the beginning of this list reversed
		/// @return An iterator for the beginning of this list reversed
		constexpr ReverseConstIterator rbegin() const noexcept { return crbegin(); }

		/// @brief Gets an iterator for the beginning of this list reversed
		/// @return An iterator for the beginning of this list reversed
		constexpr ReverseConstIterator crbegin() const noexcept { return _list.crbegin(); }

		/// @brief Gets an iterator for the end of this list reversed
		/// @return An iterator for the end of this list reversed
		constexpr ReverseIterator rend() noexcept { return _list.rend(); }

		/// @brief Gets an iterator for the end of this list reversed
		/// @return An iterator for the end of this list reversed
		constexpr ReverseConstIterator rend() const noexcept { return crend(); }

		/// @brief Gets an iterator for the end of this list reversed
		/// @return An iterator for the end of this list reversed
		constexpr ReverseConstIterator crend() const noexcept { return _list.crend(); }

		ValueType& operator[](uint64_t index) { return _list.at(index); }
		const ValueType& operator[](uint64_t index) const { return _list.at(index); }
	};
}