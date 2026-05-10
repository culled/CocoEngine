//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_PTRS_H
#define COCOENGINE_PTRS_H

#include "Allocators/STLAllocator.h"
#include "Allocator.h"
#include "MemoryOverrides.h"

#include <memory>

namespace Coco
{
    /// @brief A deleter for a UniquePtr
    /// @tparam ValueType The type of value
    template<typename ValueType>
    struct UniquePtrDeleter
    {
        /// @brief The allocator used to create the instance
        Allocator* Alloc;

        /// @brief The size of the instance
        uint64 PtrSize;

        UniquePtrDeleter() :
            Alloc(nullptr),
            PtrSize(sizeof(ValueType))
        {}

        UniquePtrDeleter(Allocator& alloc, uint64 ptrSize) :
            Alloc(&alloc),
            PtrSize(ptrSize)
        {}

        template<typename OtherType>
        UniquePtrDeleter(const UniquePtrDeleter<OtherType>& other) :
            Alloc(other.Alloc),
            PtrSize(other.PtrSize)
        {}

        template<typename OtherType>
        UniquePtrDeleter(UniquePtrDeleter<OtherType>&& other) noexcept :
            Alloc(other.Alloc),
            PtrSize(other.PtrSize)
        {}

        void operator()(ValueType* ptr) noexcept
        {
            if(Alloc)
                Delete<ValueType>(*Alloc, ptr, PtrSize);
            else
                DefaultDelete<ValueType>(ptr, PtrSize);
        }
    };

    /// @brief A smart pointer that exclusively manages the lifetime of an object
    /// @tparam ClassType The type of value
    template<typename ClassType>
    using UniquePtr = std::unique_ptr<ClassType, UniquePtrDeleter<ClassType>>;

    /// @brief Creates a UniquePtr that manages an instance of a class using the given Allocator
    /// @tparam ValueType The type of value
    /// @tparam Args Constructor arg types
    /// @param allocator The Allocator to use
    /// @param args Args to pass to the constructor
    /// @return The UniquePtr
    template<typename ValueType, typename ... Args>
    UniquePtr<ValueType> CreateUnique(Allocator& allocator, Args&& ... args)
    {
        return UniquePtr<ValueType>(New<ValueType>(allocator, std::forward<Args>(args)...), UniquePtrDeleter<ValueType>(allocator, sizeof(ValueType)));
    }


    /// @brief Creates a UniquePtr that manages an instance of a class using the default Allocator
    /// @tparam ValueType The type of value
    /// @tparam Args Constructor arg types
    /// @param args Args to pass to the constructor
    /// @return The UniquePtr
    template<typename ValueType, typename ... Args>
    UniquePtr<ValueType> CreateDefaultUnique(Args&& ... args)
    {
        Allocator* alloc = Allocator::GetDefaultAllocator();
        return UniquePtr<ValueType>(New<ValueType>(*alloc, std::forward<Args>(args)...), UniquePtrDeleter<ValueType>(*alloc, sizeof(ValueType)));
    }

    /// @brief A smart pointer that reference counts uses of an object and destroys it when there are no more references to it
    /// @tparam ValueType The type of value
    template<typename ValueType>
    using SharedPtr = std::shared_ptr<ValueType>;

    /// @brief Creates a SharedPtr that manages an instance of a class using the given Allocator
    /// @tparam ValueType The type of value
    /// @tparam Args Constructor arg types
    /// @param allocator The Allocator to use
    /// @param args Args to pass to the constructor
    /// @return The SharedPtr
    template<typename ValueType, typename ... Args>
    SharedPtr<ValueType> CreateShared(Allocator& allocator, Args&& ... args)
    {
        return std::allocate_shared<ValueType>(STLAllocator<ValueType>(&allocator), std::forward<Args>(args)...);
    }

    /// @brief Creates a SharedPtr that manages an instance of a class using the default Allocator
    /// @tparam ValueType The type of value
    /// @tparam Args Constructor arg types
    /// @param args Args to pass to the constructor
    /// @return The SharedPtr
    template<typename ValueType, typename ... Args>
    SharedPtr<ValueType> CreateDefaultShared(Args&& ... args)
    {
        return CreateShared<ValueType>(*Allocator::GetDefaultAllocator(), std::forward<Args>(args)...);
    }
}

#endif //COCOENGINE_PTRS_H
