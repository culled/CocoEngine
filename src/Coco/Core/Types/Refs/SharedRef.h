#pragma once

#include "Ref.h"

namespace Coco
{
	/// @brief A shared reference to an object that gets destroyed when the last SharedRef that references it is destroyed
	/// @tparam ValueType The type of object
	template<typename ValueType>
	class SharedRef : public Ref<ValueType>
	{
		template<typename>
		friend class SharedRef;

		template<typename>
		friend class WeakSharedRef;

	private:
		SharedRef(std::shared_ptr<RefControlBlock> controlBlock, ValueType* resource)
		{
			this->_controlBlock = controlBlock;
			this->_resource = resource;

			this->_controlBlock->AddUse();
		}

	public:
		template<typename ... Args>
		static SharedRef Create(Args&& ... args)
		{
			SharedRef ref;
			ref._controlBlock = std::make_shared<RefControlBlock>(typeid(ValueType));
			ref._resource = new ValueType(std::forward<Args>(args)...);

			return ref;
		}

		SharedRef()
		{}

		SharedRef(const SharedRef<ValueType>& object) noexcept : Ref<ValueType>(object)
		{}

		SharedRef(SharedRef<ValueType>&& object) noexcept : Ref<ValueType>(std::move(object))
		{}

		template<typename OtherType>
		SharedRef(const SharedRef<OtherType>& object) noexcept : Ref<ValueType>(object)
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");
		}

		template<typename OtherType>
		SharedRef(SharedRef<OtherType>&& object) noexcept : Ref<ValueType>(std::move(object))
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");
		}

		SharedRef& operator=(const SharedRef& other) noexcept
		{
			Ref<ValueType>::operator=(other);

			return *this;
		}

		template<typename OtherType>
		SharedRef& operator=(const SharedRef<OtherType>& other) noexcept
		{
			Ref<ValueType>::operator=(other);

			return *this;
		}

		SharedRef& operator=(SharedRef&& other) noexcept
		{
			Ref<ValueType>::operator=(std::move(other));

			return *this;
		}

		template<typename OtherType>
		SharedRef& operator=(SharedRef<OtherType>&& other) noexcept
		{
			Ref<ValueType>::operator=(std::move(other));

			return *this;
		}

		~SharedRef() override
		{
			DestroyIfUnused();
		}

		/// @brief Causes this SharedRef to take ownership of the given resource
		/// @param resource The resource to take ownership of, or nullptr for no resource
		void Reset(ValueType* resource = nullptr) noexcept
		{
			DestroyIfUnused();

			this->_controlBlock = std::make_shared<RefControlBlock>(resource);
			this->_resource = resource;
		}

	private:
		void DestroyIfUnused()
		{
			if (!this->_controlBlock || this->GetUseCount() > 0)
				return;

			this->_controlBlock->SetResourceType(typeid(std::nullptr_t));

			if (this->_resource != nullptr)
			{
				delete this->_resource;

				this->_resource = nullptr;
			}
		}
	};

	/// @brief Creates a shared reference for a given object type
	/// @tparam ValueType The type of object
	/// @tparam ...Args Arguments for the object' constructor
	/// @param args Arguments to forward to the object's constructor
	/// @return A shared reference to the object
	template<typename ValueType, typename ... Args>
	SharedRef<ValueType> CreateSharedRef(Args&&... args)
	{
		return SharedRef<ValueType>::Create(std::forward<Args>(args)...);
		//return SharedRef<ValueType>(std::forward<Args>(args)...);
	}

	/// @brief A weak reference to a SharedRef that doesn't add to the user count unless it is locked
	template<typename ValueType>
	class WeakSharedRef
	{
	private:
		std::weak_ptr<RefControlBlock> _controlBlock;
		ValueType* _resource;

	public:
		WeakSharedRef() = default;

		WeakSharedRef(const WeakSharedRef<ValueType>& other) :
			_controlBlock(other._controlBlock),
			_resource(other._resource)
		{}

		template<typename OtherType>
		WeakSharedRef(const WeakSharedRef<OtherType>& other) :
			_controlBlock(other._controlBlock),
			_resource(static_cast<ValueType*>(other._resource))
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");
		}

		WeakSharedRef(WeakSharedRef<ValueType>&& other) :
			_controlBlock(std::move(other._controlBlock)),
			_resource(std::move(other._resource))
		{}

		template<typename OtherType>
		WeakSharedRef(WeakSharedRef<OtherType>&& other) :
			_controlBlock(std::move(other._controlBlock)),
			_resource(std::move(other._resource))
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");
		}

		WeakSharedRef(const SharedRef<ValueType>& sharedRef) noexcept : 
			_controlBlock(sharedRef._controlBlock), 
			_resource(sharedRef._resource)
		{}

		template<typename OtherType>
		WeakSharedRef(const SharedRef<OtherType>& sharedRef) noexcept :
			_controlBlock(sharedRef._controlBlock), 
			_resource(static_cast<ValueType*>(sharedRef._resource))
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");
		}

		/// @brief Creates a SharedRef that references the original resource
		/// @return A shared reference to the resource
		SharedRef<ValueType> Lock() noexcept
		{
			if (_controlBlock.expired())
				return nullptr;

			std::shared_ptr<RefControlBlock> controlBlock = _controlBlock.lock();

			if (!controlBlock->IsValid())
				return nullptr;

			return SharedRef<ValueType>(controlBlock, _resource);
		}

		/// @brief Determines if the original resource is valid and usable
		/// @return True if the resource is valid and can be used
		bool IsValid() const noexcept { return !_controlBlock.expired() && _controlBlock.lock()->IsValid(); }
	};
}