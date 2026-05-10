//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_REFS_H
#define COCOENGINE_REFS_H
#include <type_traits>

#include "Allocator.h"
#include "MemoryOverrides.h"

namespace Coco
{
    /// @brief Control block for ManagedRefs and Refs
    struct RefControlBlock
    {
    	/// @brief The allocator used to create the object memory
    	Allocator* Alloc;

        /// @brief A pointer to the managed object
        void* Ptr;

    	/// @brief The size of the object's memory
    	uint32 PtrSize;

        /// @brief The number of weak uses
        uint32 WeakUseCount;

        RefControlBlock(Allocator& alloc, void* ptr, uint32 ptrSize) noexcept;
    };

    template<typename>
	class ManagedRef;

	template<typename>
	class StackManagedRef;

	/// @brief A non-owning reference to a ManagedRef
	/// @tparam ValueType The object type
	template<typename ValueType>
	class Ref
	{
		template<typename>
		friend class ManagedRef;

		template<typename>
		friend class StackManagedRef;

		template<typename>
		friend class Ref;

		template<typename V, typename V2>
		friend bool operator==(const Ref<V>& a, const Ref<V2>& b);

	public:
		Ref() noexcept :
			_controlPtr(nullptr)
		{}

		Ref(const Ref& other) noexcept :
			Ref(other._controlPtr)
		{}

		Ref(Ref&& other) noexcept :
			_controlPtr(nullptr)
		{
			swap(*this, other);
		}

		template<typename OtherType, std::enable_if_t<std::is_base_of<ValueType, OtherType>::value, bool> = true>
		Ref(const Ref<OtherType>& other) noexcept :
			Ref(other._controlPtr)
		{}

		~Ref() noexcept
		{
			RemoveUse();
		}

		Ref& operator=(Ref rhs) noexcept
		{
			swap(*this, rhs);
			return *this;
		}

		friend void swap(Ref& a, Ref& b) noexcept
		{
			using std::swap;

			swap(a._controlPtr, b._controlPtr);
		}

		operator bool() const noexcept { return _controlPtr && _controlPtr->Ptr; }

		ValueType* operator->() noexcept { return Get(); }
		const ValueType* operator->() const noexcept { return Get(); }

		ValueType& operator*() noexcept { return *Get(); }
		const ValueType& operator*() const noexcept { return *Get(); }

		/// @brief Gets a pointer to the object
		/// @return A pointer to the object
		ValueType* Get() noexcept { return _controlPtr ? static_cast<ValueType*>(_controlPtr->Ptr) : nullptr; }

		/// @brief Gets a pointer to the object
		/// @return A pointer to the object
		const ValueType* Get() const noexcept { return _controlPtr ? static_cast<const ValueType*>(_controlPtr->Ptr) : nullptr; }

		/// @brief Gets the number of Refs referencing the ManagedRef
		/// @return The number of non-owning references
		uint32 GetUseCount() const noexcept { return _controlPtr ? _controlPtr->WeakUseCount : 0; }

		/// @brief Determines if the reference is still valid
		/// @return True if the reference is valid and can be used
		bool IsValid() const noexcept { return _controlPtr ? _controlPtr->Ptr != nullptr : false; }

		/// @brief Invalidates this Ref, making it invalid
		void Invalidate() noexcept
		{
			RemoveUse();
		}

		/// @brief Downcasts this Ref to a Ref of the given type
		/// @tparam ToType The type to cast into
		/// @return The downcasted Ref
		template<typename ToType>
		Ref<ToType> Downcast() const noexcept
		{
			static_assert(std::is_base_of<ValueType, ToType>::value, "Types are not related");
			return Ref<ToType>(_controlPtr);
		}

	private:
		RefControlBlock* _controlPtr;

		Ref(RefControlBlock* controlPtr) noexcept :
			_controlPtr(controlPtr)
		{
			AddUse();
		}

		/// @brief Adds a use to the control block
		void AddUse() noexcept
		{
			if (!_controlPtr)
				return;

			++_controlPtr->WeakUseCount;
		}

		/// @brief Removes a use from the control block
		void RemoveUse() noexcept
		{
			if (!_controlPtr)
				return;

			COCO_ASSERT(_controlPtr->WeakUseCount > 0, "Weak use count was 0");
			--_controlPtr->WeakUseCount;

			// If this Ref is the only thing referencing the control block, destroy it
			if (_controlPtr->WeakUseCount == 0 && !_controlPtr->Ptr)
			{
				Delete(*_controlPtr->Alloc, _controlPtr);
			}

			_controlPtr = nullptr;
		}
	};

	template<typename ValueType, typename ValueType2>
	bool operator==(const Ref<ValueType>& a, const Ref<ValueType2>& b)
	{
		return a._controlPtr == b._controlPtr;
	}

	/// @brief Manages the lifetime of an object, but allows non-owning Refs to point to the object
	/// @tparam ValueType The object type
	template<typename ValueType>
	class ManagedRef
	{
		template<typename ObjectType>
		friend class ManagedRef;

	public:
		ManagedRef(ValueType* ptr = nullptr, Allocator* allocator = nullptr) :
			_controlPtr(nullptr)
		{
			if (!allocator)
				allocator = Allocator::GetDefaultAllocator();

			CreateControlBlock(ptr, *allocator);
		}

		ManagedRef(ManagedRef&& other) noexcept :
			_controlPtr(nullptr)
		{
			swap(*this, other);
		}

		template<typename OtherType, std::enable_if_t<std::is_base_of<ValueType, OtherType>::value, bool> = true>
		ManagedRef(ManagedRef<OtherType>&& other) noexcept :
			_controlPtr(nullptr)
		{
			using std::swap;
			swap(_controlPtr, other._controlPtr);
		}

		~ManagedRef() noexcept
		{
			DestroyManagedObject();
		}

		ManagedRef(const ManagedRef&) = delete;

		template<typename OtherType>
		ManagedRef(const ManagedRef<OtherType>&) = delete;

		ManagedRef& operator=(const ManagedRef&) = delete;

		ManagedRef& operator=(ManagedRef&& rhs) noexcept
		{
			swap(*this, rhs);
			return *this;
		}

		template<typename OtherType, std::enable_if_t<std::is_base_of<ValueType, OtherType>::value, bool> = true>
		ManagedRef& operator=(ManagedRef<OtherType>&& rhs) noexcept
		{
			using std::swap;
			swap(_controlPtr, rhs._controlPtr);

			return *this;
		}

		friend void swap(ManagedRef& a, ManagedRef& b) noexcept
		{
			using std::swap;

			swap(a._controlPtr, b._controlPtr);
		}

		operator bool() noexcept { return _controlPtr && _controlPtr->Ptr; }

		operator Ref<ValueType>() noexcept { return Ref<ValueType>(_controlPtr); }

		ValueType* operator->() noexcept { return Get(); }
		const ValueType* operator->() const noexcept { return Get(); }

		ValueType& operator*() noexcept { return *Get(); }
		const ValueType& operator*() const noexcept { return *Get(); }

		/// @brief Gets the pointer to the managed object
		/// @return A pointer to the object
		ValueType* Get() noexcept { return _controlPtr ? static_cast<ValueType*>(_controlPtr->Ptr) : nullptr; }

		/// @brief Gets the pointer to the managed object
		/// @return A pointer to the object
		const ValueType* Get() const noexcept { return _controlPtr ? static_cast<const ValueType*>(_controlPtr->Ptr) : nullptr; }

		/// @brief Gets the number of Refs created from this ManagedRef
		/// @return The number of references (not including this one)
		uint32 GetUseCount() const noexcept { return _controlPtr ? _controlPtr->WeakUseCount : 0; }

		/// @brief Destroys the managed object
		void Invalidate() noexcept
		{
			DestroyManagedObject();
		}

		/// @brief Downcasts this ManagedRef to a Ref of the given type
		/// @tparam ToType The type to downcast to
		/// @return A Ref of the downcasted type
		template<typename ToType>
		Ref<ToType> DowncastAsRef() const noexcept
		{
			static_assert(std::is_base_of<ValueType, ToType>::value, "Types are not related");
			return Ref<ToType>(_controlPtr);
		}

	private:
		RefControlBlock* _controlPtr;

		/// @brief Creates a control block for an object
		/// @param ptr A pointer to the object
		/// @param allocator The allocator to use
		void CreateControlBlock(ValueType* ptr, Allocator& allocator)
		{
			try
			{
				_controlPtr = New<RefControlBlock>(allocator, allocator, ptr, sizeof(ValueType));
			}
			catch (...)
			{
				// Ensure the managed object doesn't leak if control block construction fails
				if (ptr)
					Delete(allocator, ptr);

				throw;
			}
		}

		/// @brief Destroys the managed object
		void DestroyManagedObject() noexcept
		{
			if (!_controlPtr)
				return;

			// Destroy the object (if it exists)
			if (_controlPtr->Ptr)
			{
				Delete(*_controlPtr->Alloc, static_cast<ValueType*>(_controlPtr->Ptr), _controlPtr->PtrSize);
				_controlPtr->Ptr = nullptr;
			}

			// Destroy the control block if there are no outside users
			if (_controlPtr->WeakUseCount == 0)
			{
				Delete(*_controlPtr->Alloc, _controlPtr);
			}

			_controlPtr = nullptr;
		}
	};

	/// @brief Creates a ManagedRef using the given allocator
	/// @tparam ValueType The object type
	/// @tparam Args The constructor argument types
	/// @param allocator The allocator to use, or nullptr to use the default allocator
	/// @param args The arguments to pass to the object's constructor
	/// @return The ManagedRef
	template<typename ValueType, typename ... Args>
	ManagedRef<ValueType> CreateManagedRef(Allocator& allocator, Args&& ... args)
	{
		ValueType* ptr = nullptr;

		try
		{
			ptr = New<ValueType>(allocator, std::forward<Args>(args)...);
			return ManagedRef<ValueType>(ptr, &allocator);
		}
		catch (...)
		{
			// Delete the memory used by the failed object
			if (ptr)
				Delete(allocator, ptr);

			throw;
		}
	}

	/// @brief Creates a ManagedRef using the default allocator
	/// @tparam ValueType The object type
	/// @tparam Args The constructor argument types
	/// @param args The arguments to pass to the object's constructor
	/// @return The ManagedRef
	template<typename ValueType, typename ... Args>
	ManagedRef<ValueType> CreateDefaultManagedRef(Args&& ... args)
	{
		return CreateManagedRef<ValueType>(*Allocator::GetDefaultAllocator(), std::forward<Args>(args)...);
	}

	/// @brief Manages the lifetime of an object, but allows non-owning Refs to the object.
	/// The object and the control structure are created on the stack, so all Refs created from this object must not outlive this object
	/// @tparam ValueType The object type
	template<typename ValueType>
	class StackManagedRef
	{
		template<typename ObjectType>
		friend class StackManagedRef;

		template<typename ObjectType>
		friend class StackRef;

	public:
		template<typename ... Args>
		StackManagedRef(Args&&... args) :
			_value(std::forward<Args>(args)...),
			_controlBlock(nullptr, &_value)
		{}

		// Since the value that this object manages lives on the stack, don't allow any moving or copying
		StackManagedRef(StackManagedRef&&) = delete;
		StackManagedRef(const StackManagedRef&) = delete;

		template<typename OtherType>
		StackManagedRef(StackManagedRef<OtherType>&&) = delete;

		template<typename OtherType>
		StackManagedRef(const StackManagedRef<OtherType>&) = delete;

		StackManagedRef& operator=(StackManagedRef&&) = delete;
		StackManagedRef& operator=(const StackManagedRef&) = delete;

		template<typename OtherType>
		StackManagedRef& operator=(StackManagedRef<OtherType>&&) = delete;

		template<typename OtherType>
		StackManagedRef& operator=(const StackManagedRef<OtherType>&) = delete;

		operator Ref<ValueType>() noexcept { return Ref<ValueType>(&_controlBlock); }

		ValueType* operator->() noexcept { return Get(); }
		const ValueType* operator->() const noexcept { return Get(); }

		ValueType& operator*() noexcept { return *Get(); }
		const ValueType& operator*() const noexcept { return *Get(); }

		/// @brief Gets the pointer to the managed object
		/// @return A pointer to the object
		ValueType* Get() noexcept { return &_value; }

		/// @brief Gets the pointer to the managed object
		/// @return A pointer to the object
		const ValueType* Get() const noexcept { return &_value; }

		/// @brief Gets the number of Refs created from this ManagedRef
		/// @return The number of references (not including this one)
		uint32 GetUseCount() const noexcept { return _controlBlock.WeakUseCount; }

		/// @brief Downcasts this ManagedRef to a Ref of the given type
		/// @tparam ToType The type to downcast to
		/// @return A Ref of the downcasted type
		template<typename ToType>
		Ref<ToType> DowncastAsRef() const noexcept
		{
			static_assert(std::is_base_of<ValueType, ToType>::value, "Types are not related");
			return Ref<ToType>(&_controlBlock);
		}

	private:
		ValueType _value;
		RefControlBlock _controlBlock;
	};
}

#endif //COCOENGINE_REFS_H