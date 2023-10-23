#pragma once

#include "../Corepch.h"
#include "../Defines.h"

namespace Coco
{
	/// @brief A wrapper for std::shared_ptr
	/// @tparam ValueType The type of object
	template<typename ValueType>
	using SharedRef = std::shared_ptr<ValueType>;

	/// @brief A wrapper for std::weak_ptr
	/// @tparam ValueType The type of object
	template<typename ValueType>
	using WeakSharedRef = std::weak_ptr<ValueType>;

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

	/// @brief A managed object that can give out tracking references
	/// @tparam ValueType The type of value
	template<typename ValueType>
	class ManagedRef
	{
		template<typename RefType>
		friend class Ref;

		template<typename RefType>
		friend class ManagedRef;

	private:
		UniqueRef<ValueType> _instance;
		SharedRef<bool> _state;

	public:
		ManagedRef() :
			_instance(nullptr),
			_state(nullptr)
		{}

		ManagedRef(ValueType* ptr) :
			_instance(ptr),
			_state(CreateSharedRef<bool>(true))
		{}

		template<typename OtherType>
		ManagedRef(ManagedRef<OtherType>&& other) :
			_instance(std::move(other._instance)),
			_state(std::move(other._state))
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");
		}

		template<typename OtherType>
		ManagedRef(const ManagedRef<OtherType>&) = delete;

		~ManagedRef()
		{
			Invalidate();
		}

		template<typename OtherType>
		ManagedRef& operator=(const ManagedRef<OtherType>&) = delete;

		template<typename OtherType>
		ManagedRef& operator=(ManagedRef<OtherType>&& other)
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");

			_instance.swap(other._instance);
			_state.swap(other._state);
			return *this;
		}

		ValueType* operator->() { return _instance.get(); }
		const ValueType* operator->() const { return _instance.get(); }

		ValueType& operator*() { return *_instance; }
		const ValueType& operator*() const { return *_instance; }

		operator bool() const
		{
			return IsValid();
		}

		/// @brief Invalidates the managed object and all references to it
		void Invalidate()
		{
			if (_state)
				*_state = false;
			_instance.reset();
		}

		/// @brief Gets the managed instance
		/// @return The managed instance
		ValueType* Get() const { return _instance.get(); }

		/// @brief Determines if the managed instance is valid
		/// @return True if the instance is valid
		bool IsValid() const { return _state ? *_state : false; }

		/// @brief Gets the number of references to the instance (including this one)
		/// @return The number of references
		uint64 GetUseCount() const { return _state ? _state.use_count() : 0; }
	};

	/// @brief Creates a ManagedRef
	/// @tparam ValueType The type of value
	/// @tparam Args The types of arguments
	/// @param args Arguments to pass to the object's constructor
	/// @return A ManagedRef
	template<typename ValueType, typename ... Args>
	ManagedRef<ValueType> CreateManagedRef(Args&& ... args)
	{
		return ManagedRef(new ValueType(std::forward<Args>(args)...));
	}

	/// @brief A reference to a managed object
	/// @tparam ValueType The type of value
	template<typename ValueType>
	class Ref
	{
		template<typename OtherType>
		friend class Ref;

	private:
		ValueType* _ptr;
		SharedRef<bool> _state;

	private:
		Ref(ValueType* ptr, SharedRef<bool> state) :
			_ptr(ptr),
			_state(state)
		{}

	public:
		Ref() : 
			Ref(nullptr, nullptr)
		{}

		Ref(ValueType* ptr) :
			Ref(ptr, CreateSharedRef<bool>(ptr != nullptr))
		{}

		template<typename RefType>
		Ref(const ManagedRef<RefType>& ref) :
			Ref(static_cast<ValueType*>(ref.Get()), ref._state)
		{
			static_assert(std::is_same<RefType, ValueType>::value ||
				std::is_base_of<ValueType, RefType>::value ||
				std::is_base_of<RefType, ValueType>::value,
				"Cannot convert reference types");
		}

		template<typename RefType>
		Ref(Ref<RefType>&& ref) :
			Ref(static_cast<ValueType*>(ref._ptr), ref._state)
		{
			static_assert(std::is_same<RefType, ValueType>::value ||
				std::is_base_of<ValueType, RefType>::value ||
				std::is_base_of<RefType, ValueType>::value,
				"Cannot convert reference types");
		}

		~Ref()
		{
			Invalidate();
		}

		template<typename RefType>
		Ref& operator=(const ManagedRef<RefType>& ref)
		{
			static_assert(std::is_same<RefType, ValueType>::value || 
				std::is_base_of<ValueType, RefType>::value || 
				std::is_base_of<RefType, ValueType>::value, 
				"Cannot convert reference types");

			_ptr = static_cast<ValueType*>(ref.Get());
			_state = ref._state;

			return *this;
		}

		ValueType* operator->() { return _ptr; }
		const ValueType* operator->() const { return _ptr; }

		ValueType& operator*() { return *_ptr; }
		const ValueType& operator*() const { return *_ptr; }


		template<typename RefType>
		operator Ref<RefType>() const
		{
			static_assert(std::is_same<RefType, ValueType>::value ||
				std::is_base_of<ValueType, RefType>::value ||
				std::is_base_of<RefType, ValueType>::value,
				"Cannot convert reference types");

			return Ref<RefType>(static_cast<RefType*>(_ptr), _state);
		}

		bool operator==(const ManagedRef<ValueType>& other) const
		{
			return other.Get() == _ptr;
		}

		bool operator==(const Ref& other) const
		{
			return other.Get() == _ptr;
		}

		operator bool() const
		{
			return IsValid();
		}

		/// @brief Invalidates this reference
		void Invalidate()
		{
			if(_state)
				_state.reset();
			_ptr = nullptr;
		}

		/// @brief Gets the managed instance
		/// @return The managed instance
		ValueType* Get() const { return _ptr; }

		/// @brief Determines if the managed instance is valid
		/// @return True if the instance is valid
		bool IsValid() const { return _state ? *_state : false; }

		/// @brief Gets the number of references to the instance (including this one)
		/// @return The number of references
		uint64 GetUseCount() const { return _state ? _state.use_count() : 0; }
	};
}