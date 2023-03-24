#pragma once

#include <Coco/Core/API.h>

#include <memory>

namespace Coco
{
	template <typename>
	class ManagedRef;

	/// @brief Shared control block for a ManagedRefs and WeakManagedRefs
	class ManagedRefControlBlock
	{
		template<typename>
		friend class ManagedRef;

		bool _resourceValid = false;

	public:
		ManagedRefControlBlock();
		virtual ~ManagedRefControlBlock();

		ManagedRefControlBlock(const ManagedRefControlBlock&) = delete;
		ManagedRefControlBlock(ManagedRefControlBlock&&) = delete;
		ManagedRefControlBlock& operator=(const ManagedRefControlBlock&) = delete;
		ManagedRefControlBlock& operator=(ManagedRefControlBlock&&) = delete;

		/// @brief Gets if the resource is still valid
		/// @return True if the resource is still valid
		constexpr bool IsValid() const noexcept { return _resourceValid; }

	private:
		/// @brief Marks the resource as invalid
		constexpr void Invalidate() noexcept { _resourceValid = false; }
	};

	template <typename>
	class WeakManagedRef;

	/// @brief Class that manages the lifetime of an object, but can have weak references to the object
	/// @tparam ValueType 
	template<typename ValueType>
	class COCOAPI ManagedRef
	{
		template<typename>
		friend class ManagedRef;

		template<typename>
		friend class WeakManagedRef;

	private:
		std::shared_ptr<ManagedRefControlBlock> _controlBlock = nullptr;
		std::unique_ptr<ValueType> _managedResource;

	public:
		ManagedRef(ValueType* object) : _managedResource(object), _controlBlock(std::make_shared<ManagedRefControlBlock>())
		{}

		ManagedRef(ValueType&& object) :  _managedResource(object), _controlBlock(std::make_shared<ManagedRefControlBlock>())
		{}

		ManagedRef(std::unique_ptr<ValueType> object) : _managedResource(std::move(object)), _controlBlock(std::make_shared<ManagedRefControlBlock>())
		{}

		template<typename ObjectV>
		ManagedRef(ManagedRef<ObjectV>&& object) :
			_managedResource(std::move(object._managedResource)), _controlBlock(std::move(object._controlBlock))
		{}

		virtual ~ManagedRef()
		{
			if(_controlBlock)
				_controlBlock->Invalidate();

			_controlBlock.reset();
			_managedResource.reset();
		}

		ManagedRef(const ManagedRef&) = delete;
		ManagedRef& operator=(const ManagedRef&) = delete;

		ManagedRef& operator=(ManagedRef&& other) noexcept
		{
			_managedResource = std::move(other._managedResource);
			_controlBlock = std::move(other._controlBlock);

			return *this;
		}

		/// @brief Gets the raw resource
		/// @tparam ValueType 
		/// @return The raw resource
		ValueType* Get() const noexcept { return _managedResource.get(); }

		/// @brief Gets the number of references to this managed resource
		/// @return The number of references to this managed resource
		int GetUseCount() const noexcept { return _controlBlock.use_count() - 1; }

		ValueType* operator->()const noexcept { return _managedResource.get(); }
	};

	/// @brief Creates a managed reference for a given type
	/// @tparam ValueType
	/// @tparam ...Args
	/// @param managedRef Arguments to forward to the object's constructor
	/// @return A managed reference
	template<typename ValueType, typename ... Args>
	ManagedRef<ValueType> CreateManagedRef(Args&&... args)
	{
		return ManagedRef<ValueType>(CreateManaged<ValueType>(std::forward<Args>(args)...));
	}

	/// @brief A weak reference to a ManagedRef object
	/// @tparam ValueType 
	template<typename ValueType>
	class COCOAPI WeakManagedRef
	{
		template<typename>
		friend class WeakManagedRef;

	private:
		std::shared_ptr<ManagedRefControlBlock> _controlBlock;
		ValueType* _ptr;

	public:
		WeakManagedRef() :
			_controlBlock(nullptr), _ptr(nullptr)
		{}

		template<typename ObjectV>
		WeakManagedRef(const ManagedRef<ObjectV>& managedRef) :
			_controlBlock(managedRef._controlBlock), _ptr(static_cast<ValueType*>(managedRef.Get()))
		{}

		WeakManagedRef(const WeakManagedRef& weakRef) : 
			_controlBlock(weakRef._controlBlock), _ptr(weakRef._ptr)
		{}

		template<typename ObjectV>
		WeakManagedRef(const WeakManagedRef<ObjectV>& weakRef) :
			_controlBlock(weakRef._controlBlock), _ptr(static_cast<ValueType*>(weakRef._ptr))
		{}

		WeakManagedRef(WeakManagedRef&& weakRef) noexcept : 
			_controlBlock(std::move(weakRef._controlBlock)), _ptr(std::move(weakRef._ptr))
		{}

		template<typename ObjectV>
		WeakManagedRef(WeakManagedRef<ObjectV>&& weakRef) noexcept :
			_controlBlock(std::move(weakRef._controlBlock)), _ptr(std::move(static_cast<ValueType*>(weakRef._ptr)))
		{}

		virtual ~WeakManagedRef()
		{
			Invalidate();
		}

		/// @brief Gets the raw resource
		/// @return The raw resource, or nullptr if the resource is invalid
		ValueType* Get() const noexcept
		{
			if (IsValid())
			{
				Assert(_ptr != nullptr);
				return _ptr;
			}

			return nullptr;
		}

		/// @brief Gets if the resource is valid
		/// @return True if the resource is valid
		bool IsValid() const noexcept { return _controlBlock && _controlBlock->IsValid(); }

		/// @brief Invalidates this weak reference
		void Invalidate() noexcept
		{
			_controlBlock.reset();
			_ptr = nullptr;
		}

		WeakManagedRef& operator=(const WeakManagedRef& other) noexcept
		{
			_controlBlock = other._controlBlock;
			_ptr = other._ptr;

			return *this;
		}

		WeakManagedRef& operator=(WeakManagedRef&& other) noexcept
		{
			_controlBlock = std::move(other._controlBlock);
			_ptr = std::move(other._ptr);

			return *this;
		}

		ValueType* operator->() const noexcept { return _ptr; }
	};

	/// @brief Casts a weak managed ref
	/// @tparam To 
	/// @tparam From 
	/// @param from The original reference
	/// @return The casted reference
	template<typename To, typename From>
	WeakManagedRef<To> WeakManagedRefCast(const WeakManagedRef<From>& from)
	{
		return WeakManagedRef<To>(from);
	}
}