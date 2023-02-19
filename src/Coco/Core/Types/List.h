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
		List(int initialSize) : _list(initialSize) {}

		/// <summary>
		/// Adds an item to this list
		/// </summary>
		/// <param name="item">The item to add</param>
		void Add(T&& item)
		{
			_list.push_back(std::forward<T>(item));
		}

		/// <summary>
		/// Adds an item to this list
		/// </summary>
		/// <param name="item">The item to add</param>
		void Add(const T& item)
		{
			_list.push_back(item);
		}

		void Insert(int index, T&& item)
		{
			if (index < 0 || index > Count())
			{
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));
			}

			_list.insert(begin() + index, std::forward<T>(item));
		}

		void Insert(int index, const T& item)
		{
			if (index < 0 || index > Count())
			{
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));
			}

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
		int Count() const
		{
			return static_cast<int>(_list.size());
		}

		T& operator[](int index)
		{
			if (index < 0 || index > Count() - 1)
			{
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));
			}

			return _list[index];
		}

		const T& operator[](int index) const
		{
			if (index < 0 || index > Count() - 1)
			{
				throw IndexOutOfRangeException(FormattedString("Index must be 0 <= {0} < {1}", index, Count()));
			}

			return _list[index];
		}

		/// <summary>
		/// Resizes this list to be the given size
		/// </summary>
		/// <param name="newSize">The new size of this list</param>
		void Resize(int newSize)
		{
			_list.resize(newSize);
		}

		const T* Data() const
		{
			return _list.data();
		}

		T* Data()
		{
			return _list.data();
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
		ConstIterator cbegin() const { return _list.cbegin(); }

		ConstIterator begin() const { return cbegin(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		Iterator end() { return _list.end(); }

		/// <summary>
		/// Gets an iterator for the end of this list
		/// </summary>
		/// <returns>An iterator for the end of this list</returns>
		ConstIterator cend() const { return _list.cend(); }

		ConstIterator end() const { return cend(); }
	};
}