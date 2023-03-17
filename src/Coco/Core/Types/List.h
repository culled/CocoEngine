#pragma once

#include <vector>
#include <functional>

namespace Coco
{
	/// <summary>
	/// A resizable array of items
	/// </summary>
	template<typename T>
	class COCOAPI List
	{
	private:
		std::vector<T> _list;

	public:
		using Iterator = std::vector<T>::iterator;
		using ConstIterator = std::vector<T>::const_iterator;
		using ReverseIterator = std::vector<T>::reverse_iterator;
		using ConstReverseIterator = std::vector<T>::const_reverse_iterator;

		List() noexcept = default;
		virtual ~List() = default;

		List(uint64_t initialSize) : _list(initialSize) {}
		List(std::initializer_list<T>&& items) : _list(items) {}

		/// <summary>
		/// Adds an item to this list
		/// </summary>
		/// <param name="item">The item to add</param>
		constexpr void Add(T&& item) { _list.push_back(std::forward<T>(item)); }

		/// <summary>
		/// Adds an item to this list
		/// </summary>
		/// <param name="item">The item to add</param>
		constexpr void Add(const T& item) { _list.push_back(item); }

		/// <summary>
		/// Inserts an item at the specified index
		/// </summary>
		/// <param name="index">The index that the item will be insterted at</param>
		/// <param name="item">The item to insert</param>
		void Insert(uint64_t index, T&& item)
		{
			if (index < 0 || index > Count())
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} <= {1}", index, Count()));

			_list.insert(begin() + index, std::forward<T>(item));
		}

		/// <summary>
		/// Inserts an item at the specified index
		/// </summary>
		/// <param name="index">The index that the item will be insterted at</param>
		/// <param name="item">The item to insert</param>
		void Insert(uint64_t index, const T& item)
		{
			if (index < 0 || index > Count())
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} <= {1}", index, Count()));

			_list.insert(begin() + index, item);
		}

		/// <summary>
		/// Removes the first occurance of an item from this list
		/// </summary>
		/// <param name="item">The item to remove</param>
		/// <returns>True if the item was found and removed</returns>
		bool Remove(const T& item) noexcept
		{
			Iterator it = Find(item);

			if (it != end())
				return Remove(it);

			return false;
		}

		/// <summary>
		/// Remove an item at the given iterator
		/// </summary>
		/// <param name="it">The iterator</param>
		/// <returns>True if the item was erased successfully</returns>
		bool Remove(Iterator it) noexcept
		{
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

		/// <summary>
		/// Erases an item at the given iterator and returns the element after the one erased or the list end if the element was the last one erased
		/// </summary>
		/// <param name="it">The iterator</param>
		/// <returns>An iterator to the element after the one erased or the list end if the element was the last one erased</returns>
		constexpr Iterator EraseAndGetNext(Iterator it) { return _list.erase(it); }

		/// <summary>
		/// Removes all items from this list
		/// </summary>
		constexpr void Clear() noexcept { _list.clear(); }

		/// <summary>
		/// Gets the number of items in this list
		/// </summary>
		/// <returns>The number of items in this list</returns>
		constexpr uint64_t Count() const noexcept { return _list.size(); }

		/// <summary>
		/// Gets the first element in the list
		/// </summary>
		/// <returns>The first element of the list</returns>
		constexpr T& First() noexcept { return _list.front(); }

		/// <summary>
		/// Gets the first element in the list
		/// </summary>
		/// <returns>The first element of the list</returns>
		constexpr const T& First() const noexcept { return _list.front(); }

		/// <summary>
		/// Gets the last element in the list
		/// </summary>
		/// <returns>The last element in the list</returns>
		constexpr T& Last() noexcept { return _list.back(); }

		/// <summary>
		/// Gets the last element in the list
		/// </summary>
		/// <returns>The last element in the list</returns>
		constexpr const T& Last() const noexcept { return _list.back(); }

		/// <summary>
		/// Resizes this list to be the given size
		/// </summary>
		/// <param name="newSize">The new size of this list</param>
		constexpr void Resize(uint64_t newSize) { _list.resize(newSize); }

		/// <summary>
		/// Gets a pointer to this list's first item
		/// </summary>
		/// <returns>A pointer to this list's first item</returns>
		constexpr const T* Data() const noexcept { return _list.data(); }

		/// <summary>
		/// Gets a pointer to this list's first item
		/// </summary>
		/// <returns>A pointer to this list's first item</returns>
		constexpr T* Data() noexcept { return _list.data(); }

		/// <summary>
		/// Determines if a given element exists in this list
		/// </summary>
		/// <param name="item">The item to search for</param>
		/// <returns>True if the item is found in this list</returns>
		bool Contains(const T& item) const noexcept
		{
			if (Find(item) != end())
				return true;

			return false;
		}

		/// <summary>
		/// Tries to find the given item in this list
		/// </summary>
		/// <param name="item">The item to find</param>
		/// <returns>An iterator to the item if found, else the iterator will equal the end iterator</returns>
		constexpr Iterator Find(const T& item)
		{
			return std::find(begin(), end(), item);
		}

		/// <summary>
		/// Tries to find the given item in this list
		/// </summary>
		/// <param name="item">The item to find</param>
		/// <returns>An iterator to the item if found, else the iterator will equal the end iterator</returns>
		constexpr ConstIterator Find(const T& item) const
		{
			return std::find(cbegin(), cend(), item);
		}

		/// <summary>
		/// Tries to find the given item in this list using a predicate function
		/// </summary>
		/// <param name="predicate">The function to test if the item is found</param>
		/// <returns>An iterator to the item if found, else the iterator will equal the end iterator</returns>
		constexpr Iterator Find(std::function<bool(const T&)> predicate)
		{
			return std::find_if(begin(), end(), predicate);
		}

		/// <summary>
		/// Tries to find the given item in this list using a predicate function
		/// </summary>
		/// <param name="predicate">The function to test if the item is found</param>
		/// <returns>An iterator to the item if found, else the iterator will equal the end iterator</returns>
		constexpr ConstIterator Find(std::function<bool(const T&)> predicate) const
		{
			return std::find_if(cbegin(), cend(), predicate);
		}

		/// <summary>
		/// Gets an iterator for the beginning of this list
		/// </summary>
		/// <returns>An iterator for the beginning of this list</returns>
		constexpr Iterator begin() noexcept { return _list.begin(); }

		/// <summary>
		/// Gets an iterator for the beginning of this list
		/// </summary>
		/// <returns>An iterator for the beginning of this list</returns>
		constexpr ConstIterator begin() const noexcept { return cbegin(); }

		/// <summary>
		/// Gets an iterator for the beginning of this list
		/// </summary>
		/// <returns>An iterator for the beginning of this list</returns>
		constexpr ConstIterator cbegin() const noexcept { return _list.cbegin(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		constexpr Iterator end() noexcept { return _list.end(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		constexpr ConstIterator end() const noexcept { return cend(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		constexpr ConstIterator cend() const noexcept { return _list.cend(); }

		/// <summary>
		/// Gets an iterator for the beginning of this list reversed
		/// </summary>
		/// <returns>An iterator for the beginning of this list reversed</returns>
		constexpr ReverseIterator rbegin() noexcept { return _list.rbegin(); }

		/// <summary>
		/// Gets an iterator for the beginning of this list reversed
		/// </summary>
		/// <returns>An iterator for the beginning of this list reversed</returns>
		constexpr ConstReverseIterator rbegin() const noexcept { return crbegin(); }

		/// <summary>
		/// Gets an iterator for the beginning of this list reversed
		/// </summary>
		/// <returns>An iterator for the beginning of this list reversed</returns>
		constexpr ConstReverseIterator crbegin() const noexcept { return _list.crbegin(); }

		/// <summary>
		/// Gets an iterator for the end of this list reversed
		/// </summary>
		/// <returns>An iterator for the end of this list reversed</returns>
		constexpr ReverseIterator rend() noexcept { return _list.rend(); }

		/// <summary>
		/// Gets an iterator for the end of this list reversed
		/// </summary>
		/// <returns>An iterator for the end of this list reversed</returns>
		constexpr ConstReverseIterator rend() const noexcept { return crend(); }

		/// <summary>
		/// Gets an iterator for the end of this list reversed
		/// </summary>
		/// <returns>An iterator for the end of this list reversed</returns>
		constexpr ConstReverseIterator crend() const noexcept { return _list.crend(); }

		T& operator[](uint64_t index) { return _list.at(index); }
		const T& operator[](uint64_t index) const { return _list.at(index); }
	};
}