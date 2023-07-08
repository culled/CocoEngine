#pragma once

#include <Coco/Core/API.h>

#include "RefControlBlock.h"
#include <functional>

namespace Coco
{
	/// @brief A reference to a type of object
	/// @tparam ValueType The object type
	template<typename ValueType>
	class Ref
	{
		template<typename>
		friend class Ref;
	public:
		using DeleterType = std::function<void(ValueType*)>;

	protected:
		std::shared_ptr<RefControlBlock> _controlBlock = nullptr;
		ValueType* _resource = nullptr;

	public:
		Ref() = default;

		Ref(const Ref& other) noexcept : 
			_controlBlock(other._controlBlock), 
			_resource(other._resource)
		{
			if(_controlBlock)
				_controlBlock->AddUse();
		}
		
		Ref(Ref&& other) noexcept : 
			_controlBlock(std::move(other._controlBlock)), 
			_resource(std::move(other._resource))
		{}

		template<typename OtherType>
		Ref(const Ref<OtherType>& other) noexcept : 
			_controlBlock(other._controlBlock), 
			_resource(static_cast<ValueType*>(other._resource))
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");

			if (_controlBlock)
				_controlBlock->AddUse();
		}

		template<typename OtherType>
		Ref(Ref<OtherType>&& other) noexcept : 
			_controlBlock(std::move(other._controlBlock)), 
			_resource(std::move(static_cast<ValueType*>(other._resource)))
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");
		}

		virtual ~Ref()
		{
			if (!_controlBlock)
				return;

			_controlBlock->RemoveUse();
		}

		Ref& operator=(const Ref& other) noexcept
		{
			if (_controlBlock)
				_controlBlock->RemoveUse();

			_controlBlock = other._controlBlock;
			_resource = other._resource;

			if (_controlBlock)
				_controlBlock->AddUse();

			return *this;
		}

		Ref& operator=(Ref&& other) noexcept
		{
			_controlBlock = std::move(other._controlBlock);
			_resource = std::move(other._resource);

			return *this;
		}

		template<typename OtherType>
		Ref& operator=(const Ref<OtherType>& other) noexcept
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");

			if (_controlBlock)
				_controlBlock->RemoveUse();

			_controlBlock = other._controlBlock;
			_resource = static_cast<ValueType*>(other._resource);

			if (_controlBlock)
				_controlBlock->AddUse();

			return *this;
		}

		template<typename OtherType>
		Ref& operator=(Ref<OtherType>&& other) noexcept
		{
			static_assert(std::is_base_of<ValueType, OtherType>::value || std::is_base_of<OtherType, ValueType>::value, "Cannot convert reference types");

			_controlBlock = std::move(other._controlBlock);
			_resource = std::move(static_cast<ValueType*>(other._resource));

			return *this;
		}

		/// @brief Gets the raw resource
		/// @return A pointer to the resource
		ValueType* Get() noexcept { return _resource; }

		/// @brief Gets the raw resource
		/// @return A pointer to the resource
		const ValueType* Get() const noexcept { return _resource; }

		ValueType* operator->() noexcept { return _resource; }
		const ValueType* operator->() const noexcept { return _resource; }

		ValueType& operator*() noexcept { return *_resource; }
		const ValueType& operator*() const noexcept { return *_resource; }

		template<typename OtherType>
		constexpr bool operator<(const Ref<OtherType>& other) noexcept { return _controlBlock < other._controlBlock; }

		template<typename OtherType>
		constexpr bool operator>(const Ref<OtherType>& other) noexcept { return _controlBlock > other._controlBlock; }

		template<typename OtherType>
		constexpr bool operator==(const Ref<OtherType>& other) noexcept { return this == &other; }

		template<typename OtherType>
		constexpr bool operator!=(const Ref<OtherType>& other) noexcept { return this != &other; }

		/// @brief Gets if the resource is still valid
		/// @return True if the resource is still valid and safe to use
		bool IsValid() const noexcept { return _controlBlock && _controlBlock->IsValid() && _resource != nullptr; }

		/// @brief Gets the number of references to the resource
		/// @return The number of references to the resource
		uint64_t GetUseCount() const noexcept { return (_controlBlock && _controlBlock->IsValid()) ? _controlBlock->GetUseCount() : 0; }

		operator bool() noexcept { return IsValid(); }

		operator ValueType* () { return Get(); }
	};
}