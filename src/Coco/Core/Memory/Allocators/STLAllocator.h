//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_STLALLOCATOR_H
#define COCOENGINE_STLALLOCATOR_H
#include "../Allocator.h"
#include "Coco/Core/Asserts.h"

namespace Coco
{

    /// @brief A wrapper that enables the use of an Allocator with an STL type
    /// @tparam ValueType The type of object being allocated
    template<typename ValueType>
    class STLAllocator
    {
        template<typename U>
        friend class STLAllocator;

    public:
        using pointer = ValueType*;
        using const_pointer = const ValueType*;

        using void_pointer = void*;
        using const_void_pointer = const void*;

        using value_type = ValueType;

        using size_type = uint64;
        using difference_type = std::ptrdiff_t;

        STLAllocator(Allocator* allocator = nullptr) :
            _allocator(allocator ? allocator : Allocator::GetDefaultAllocator())
        {
            COCO_ASSERT(_allocator, "Allocator was null");
        }

        template<typename U>
        STLAllocator(const STLAllocator<U>& other) :
            _allocator(other._allocator)
        {}

        template<typename U>
        STLAllocator(STLAllocator<U>&& other) noexcept :
            _allocator(other._allocator)
        {}

        template<typename U>
        bool operator==(const STLAllocator<U>& rhs) const noexcept
        {
            return _allocator == rhs._allocator;
        }

        /// @brief Allocates memory
        /// @param count The number of objects to allocate memory for
        /// @return The allocated memory
        constexpr ValueType* allocate(std::size_t count)
        {
            return static_cast<ValueType*>(_allocator->Allocate(sizeof(ValueType) * count));
        }

        /// @brief Frees memory
        /// @param ptr The memory to free
        /// @param count The number of objects that the original memory was allocated for
        constexpr void deallocate(pointer ptr, std::size_t count) noexcept
        {
            _allocator->Free(ptr, sizeof(ValueType) * count);
        }

    private:
        Allocator* _allocator;
    };
}
#endif //COCOENGINE_STLALLOCATOR_H