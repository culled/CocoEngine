#pragma once

#include "Ref.h"

namespace Coco
{
	/// @brief Class that controls the lifetime of an object, but can distribute non-owning references to it
	/// @tparam ValueType The object's type
	template<typename ValueType, typename Deleter = std::default_delete<ValueType>>
	class ManagedRef : public Ref<ValueType>
	{
		template<typename, typename>
		friend class ManagedRef;

	public:
		template<typename ... Args>
		static ManagedRef Create(Args&& ... args)
		{
			ManagedRef ref;
			ref._controlBlock = std::make_shared<RefControlBlock>(typeid(ValueType));
			ref._resource = new ValueType(std::forward<Args>(args)...);

			return ref;
		}

		ManagedRef() = default;

		ManagedRef(ManagedRef&& other) noexcept : Ref<ValueType>(std::move(other))
		{}

		template<typename OtherType>
		ManagedRef(ManagedRef<OtherType>&& other) noexcept : Ref<ValueType>(std::move(other))
		{}

		ManagedRef& operator=(ManagedRef&& other) noexcept
		{
			Ref<ValueType>::operator=(std::move(other));

			return *this;
		}

		template<typename OtherType>
		ManagedRef& operator=(ManagedRef<OtherType>&& other) noexcept
		{
			Ref<ValueType>::operator=(std::move(other));

			return *this;
		}
		~ManagedRef() noexcept override
		{
			// Since we own the resource, invalidate it when this object is destroyed
			DestroyResource();
		}

		ManagedRef(const ManagedRef&) = delete;
		ManagedRef& operator=(const ManagedRef&) = delete;


		operator Ref<ValueType>() noexcept
		{
			return Ref<ValueType>(*this);
		}

		/// @brief Causes this ManagedRef to invalidate its old resource and take ownership of the given resource. 
		/// Causes any Refs created from this ManagedRef to point to the new resource
		/// @param resource The resource to take ownership of
		void Reset(ValueType* resource = nullptr) noexcept
		{
			DestroyResource();

			this->_controlBlock->SetResourceType(typeid(ValueType));
			this->_resource = resource;
		}

		/// @brief Causes this ManagedRef to release ownership of its resource without destroying it. 
		/// Invalidates any Refs created from this ManagedRef
		void Release() noexcept
		{
			this->_controlBlock->SetResourceType(typeid(std::nullptr_t));
			this->_resource = nullptr;
		}

	private:
		/// @brief Destroys the managed resource
		void DestroyResource() noexcept
		{
			if (!this->_controlBlock)
				return;

			this->_controlBlock->SetResourceType(typeid(std::nullptr_t));

			if (this->_resource != nullptr)
			{
				Deleter deleter{};
				deleter(this->_resource);
				this->_resource = nullptr;
			}
		}
	};

	/// @brief Creates a managed reference for a given type
	/// @tparam ValueType The type of object to manage
	/// @tparam ...Args The types of arguments to pass to the object's constructor
	/// @param args Arguments to forward to the object's constructor
	/// @return A managed reference
	template<typename ValueType, typename ... Args>
	ManagedRef<ValueType> CreateManagedRef(Args&&... args)
	{
		return ManagedRef<ValueType>::Create(std::forward<Args>(args)...);
	}

	/// @brief Creates a managed reference for a given type with a defined deleter
	/// @tparam ValueType The type of object to manage
	/// @tparam Deleter The type of deleter for the object
	/// @tparam ...Args The types of arguments to pass to the object's constructor
	/// @param args Arguments to forward to the object's constructor
	/// @return A managed reference
	template<typename ValueType, typename Deleter, typename ... Args>
	ManagedRef<ValueType, Deleter> CreateManagedRefWithDeleter(Args&&... args)
	{
		return ManagedRef<ValueType, Deleter>::Create(std::forward<Args>(args)...);
	}
}