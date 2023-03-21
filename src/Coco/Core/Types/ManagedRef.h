#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	template <typename>
	class ManagedRef;

	/// <summary>
	/// Shared control block for a ManagedRefs and WeakManagedRefs
	/// </summary>
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

		/// <summary>
		/// Gets if the resource is still valid
		/// </summary>
		/// <returns>True if the resource is still valid</returns>
		bool IsValid() const noexcept { return _resourceValid; }

	private:
		/// <summary>
		/// Marks the resource as invalid
		/// </summary>
		void Invalidate() noexcept { _resourceValid = false; }
	};

	template <typename>
	class WeakManagedRef;

	/// <summary>
	/// Class that manages the lifetime of an object, but can have weak references to the object
	/// </summary>
	template<typename ObjectType>
	class COCOAPI ManagedRef
	{
		template<typename>
		friend class ManagedRef;

		template<typename>
		friend class WeakManagedRef;

	private:
		std::shared_ptr<ManagedRefControlBlock> _controlBlock = nullptr;
		std::unique_ptr<ObjectType> _managedResource;

	public:
		ManagedRef(ObjectType* object) : _managedResource(object), _controlBlock(std::make_shared<ManagedRefControlBlock>())
		{}

		ManagedRef(ObjectType&& object) :  _managedResource(object), _controlBlock(std::make_shared<ManagedRefControlBlock>())
		{}

		ManagedRef(std::unique_ptr<ObjectType> object) : _managedResource(std::move(object)), _controlBlock(std::make_shared<ManagedRefControlBlock>())
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

		ManagedRef& operator=(ManagedRef&& other)
		{
			_managedResource = std::move(other._managedResource);
			_controlBlock = std::move(other._controlBlock);

			return *this;
		}

		/// <summary>
		/// Gets the raw resource
		/// </summary>
		/// <returns>The raw resource</returns>
		ObjectType* Get() const noexcept { return _managedResource.get(); }

		/// <summary>
		/// Gets the number of references to this managed resource
		/// </summary>
		/// <returns>The number of references to this managed resource</returns>
		int GetUseCount() const noexcept { return _controlBlock.use_count() - 1; }

		ObjectType* operator->()const noexcept { return _managedResource.get(); }
	};

	/// <summary>
	/// Creates a managed reference for a given type
	/// </summary>
	/// <param name="...args">Arguments to forward to the object's constructor</param>
	/// <returns>A managed reference</returns>
	template<typename ObjectType, typename ... Args>
	ManagedRef<ObjectType> CreateManagedRef(Args&&... args)
	{
		return ManagedRef<ObjectType>(CreateManaged<ObjectType>(std::forward<Args>(args)...));
	}

	/// <summary>
	/// A weak reference to a ManagedRef object
	/// </summary>
	template<typename ObjectType>
	class COCOAPI WeakManagedRef
	{
		template<typename>
		friend class WeakManagedRef;

	private:
		std::shared_ptr<ManagedRefControlBlock> _controlBlock;
		ObjectType* _ptr;

	public:
		WeakManagedRef() :
			_controlBlock(nullptr), _ptr(nullptr)
		{}

		template<typename ObjectV>
		WeakManagedRef(const ManagedRef<ObjectV>& managedRef) :
			_controlBlock(managedRef._controlBlock), _ptr(static_cast<ObjectType*>(managedRef.Get()))
		{}

		WeakManagedRef(const WeakManagedRef& weakRef) : 
			_controlBlock(weakRef._controlBlock), _ptr(weakRef._ptr)
		{}

		template<typename ObjectV>
		WeakManagedRef(const WeakManagedRef<ObjectV>& weakRef) :
			_controlBlock(weakRef._controlBlock), _ptr(weakRef._ptr)
		{}

		WeakManagedRef(WeakManagedRef&& weakRef) : 
			_controlBlock(std::move(weakRef._controlBlock)), _ptr(std::move(weakRef._ptr))
		{}

		template<typename ObjectV>
		WeakManagedRef(WeakManagedRef<ObjectV>&& weakRef) :
			_controlBlock(std::move(weakRef._controlBlock)), _ptr(std::move(weakRef._ptr))
		{}

		virtual ~WeakManagedRef()
		{
			Invalidate();
		}

		/// <summary>
		/// Gets the raw resource
		/// </summary>
		/// <returns>The raw resource, or nullptr if the resource is invalid</returns>
		ObjectType* Get() const noexcept
		{
			if (IsValid())
			{
				Assert(_ptr != nullptr);
				return _ptr;
			}

			return nullptr;
		}

		/// <summary>
		/// Gets if the resource is valid
		/// </summary>
		/// <returns>True if the resource is valid</returns>
		bool IsValid() const noexcept { return _controlBlock && _controlBlock->IsValid(); }

		/// <summary>
		/// Invalidates this reference
		/// </summary>
		void Invalidate() noexcept
		{
			_controlBlock.reset();
			_ptr = nullptr;
		}

		WeakManagedRef& operator=(const WeakManagedRef& other)
		{
			_controlBlock = other._controlBlock;
			_ptr = other._ptr;

			return *this;
		}

		WeakManagedRef& operator=(WeakManagedRef&& other)
		{
			_controlBlock = std::move(other._controlBlock);
			_ptr = std::move(other._ptr);

			return *this;
		}

		ObjectType* operator->() const noexcept { return _ptr; }
	};
}