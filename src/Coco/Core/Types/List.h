#pragma once

#include <vector>

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

		List() = default;
		List(uint64_t initialSize) : _list(initialSize) {}
		List(std::initializer_list<T>&& items) : _list(items) {}

		/// <summary>
		/// Adds an item to this list
		/// </summary>
		/// <param name="item">The item to add</param>
		T& Add(T&& item)
		{
			return _list.emplace_back(std::forward<T>(item));
		}

		/// <summary>
		/// Adds an item to this list
		/// </summary>
		/// <param name="item">The item to add</param>
		void Add(const T& item)
		{
			_list.push_back(item);
		}

		/// <summary>
		/// Inserts an item at the specified index
		/// </summary>
		/// <param name="index">The index that the item will be insterted at</param>
		/// <param name="item">The item to insert</param>
		void Insert(uint64_t index, T&& item)
		{
			if (index < 0 || index > Count())
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));

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
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));

			_list.insert(begin() + index, item);
		}

		/// <summary>
		/// Removes the first occurance of an item from this list
		/// </summary>
		/// <param name="item">The item to remove</param>
		/// <returns>True if the item was found and removed</returns>
		bool Remove(const T& item)
		{
			Iterator it = std::find(begin(), end(), item);

			if (it != end())
			{
				_list.erase(it);
				return true;
			}

			return false;
		}

		/// <summary>
		/// Erases an item at the given iterator
		/// </summary>
		/// <param name="it">The iterator</param>
		void Erase(Iterator it)
		{
			if(it != end())
				_list.erase(it);
		}

		/// <summary>
		/// Removes all items from this list
		/// </summary>
		void Clear()
		{
			_list.clear();
		}

		/// <summary>
		/// Gets the number of items in this list
		/// </summary>
		/// <returns>The number of items in this list</returns>
		uint64_t Count() const { return _list.size(); }

		T& operator[](uint64_t index)
		{
			if (index > Count() - 1)
			{
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));
			}

			return _list[index];
		}

		const T& operator[](uint64_t index) const
		{
			if (index > Count() - 1)
			{
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));
			}

			return _list[index];
		}

		/// <summary>
		/// Resizes this list to be the given size
		/// </summary>
		/// <param name="newSize">The new size of this list</param>
		void Resize(uint64_t newSize)
		{
			_list.resize(newSize);
		}

		/// <summary>
		/// Gets a pointer to this list's first item
		/// </summary>
		/// <returns>A pointer to this list's first item</returns>
		const T* Data() const
		{
			return _list.data();
		}

		/// <summary>
		/// Gets a pointer to this list's first item
		/// </summary>
		/// <returns>A pointer to this list's first item</returns>
		T* Data()
		{
			return _list.data();
		}

		/// <summary>
		/// Determines if a given element exists in this list
		/// </summary>
		/// <param name="item">The item to search for</param>
		/// <returns>True if the item is found in this list</returns>
		bool Contains(const T& item) const
		{
			ConstIterator it = std::find(begin(), end(), item);

			if (it != end())
			{
				return true;
			}

			return false;
		}

		/// <summary>
		/// Gets an iterator for the beginning of this list
		/// </summary>
		/// <returns>An iterator for the beginning of this list</returns>
		Iterator begin() { return _list.begin(); }

		/// <summary>
		/// Gets an iterator for the beginning of this list
		/// </summary>
		/// <returns>An iterator for the beginning of this list</returns>
		ConstIterator begin() const { return cbegin(); }

		/// <summary>
		/// Gets an iterator for the beginning of this list
		/// </summary>
		/// <returns>An iterator for the beginning of this list</returns>
		ConstIterator cbegin() const { return _list.cbegin(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		Iterator end() { return _list.end(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		ConstIterator end() const { return cend(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		ConstIterator cend() const { return _list.cend(); }
	};
}