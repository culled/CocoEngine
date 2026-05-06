//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_MEMORYOVERRIDES_H
#define COCOENGINE_MEMORYOVERRIDES_H
#include <utility>
#include "../Types/CoreTypes.h"
#include "Allocator.h"
#include "Coco/Core/Asserts.h"
#include "Coco/Core/Types/Span.h"

namespace Coco
{
    class Allocator;
}

namespace Coco::Memory
{
    /// @brief Allocates memory using the given Allocator
    /// @param allocator The allocator to use
    /// @param size The number of bytes to allocate
    /// @return A pointer to the allocated block of memory
    void* Allocate(Allocator& allocator, uint64 size);

    /// @brief Frees memory allocated by the given Allocator
    /// @param allocator The allocator that allocated the memory
    /// @param ptr A pointer to the allocated block of memory
    /// @param size The size of the memory block, in bytes
    void Free(Allocator& allocator, void* ptr, uint64 size) noexcept;
}

namespace Coco
{
    /// @brief Calls the class constructor for an object
    /// @tparam ClassType The type of class
    /// @tparam Args The constructor args
    /// @param ptr A pointer to the class being constructed
    /// @param args The args to pass to the class's constructor
    template<typename ClassType, typename ... Args>
    void Construct(ClassType* ptr, Args&& ... args)
    {
        COCO_ASSERT(ptr, "Object memory was not allocated");
        new (ptr) ClassType(std::forward<Args>(args)...);
    }

    template<typename ClassType>
    void Construct(ClassType* ptr, ClassType&& other)
    {
        COCO_ASSERT(ptr, "Object memory was not allocated");
        new (ptr) ClassType(std::move(other));
    }

    /// @brief Calls the class copy-constructor for an array of objects.
    /// The number of objects is assumed to equal the number of values given
    /// @tparam ClassType The type of class
    /// @param ptr A pointer to the first class. NOTE: ensure the number of objects is greater than or equal to the number of values
    /// @param values The values to respectively copy-construct for each class
    template<typename ClassType>
    void ConstructArray(ClassType* ptr, Span<const ClassType> values)
    {
        COCO_ASSERT(ptr, "Object memory was not allocated");
        for (uint64 i = 0; i < values.size(); i++)
            new(ptr + i) ClassType(values[i]);
    }

    template<typename ClassType>
    void ConstructArray(ClassType* ptr, const ClassType& value, uint64 count)
    {
        COCO_ASSERT(ptr, "Object memory was not allocated");
        for (uint64 i = 0; i < count; i++)
            new(ptr + i) ClassType(value);
    }

    /// @brief Calls the class destructor for a given object
    /// @tparam ClassType The type of class
    /// @param ptr A pointer to the object
    template<typename ClassType>
    void Destruct(ClassType* ptr) noexcept
    {
        COCO_ASSERT(ptr, "Object memory was not allocated");
        ptr->~ClassType();
    }

    /// @brief Calls the class destructor for an array of objects
    /// @tparam ClassType The type of class
    /// @param ptr A pointer to the first object being destructed
    /// @param count The number of objects to destruct
    template<typename ClassType>
    void DestructArray(ClassType* ptr, uint64 count) noexcept
    {
        COCO_ASSERT(ptr, "Object memory was not allocated");
        for (uint64 i = 0; i < count; i++)
            (ptr + i)->~ClassType();
    }

    /// @brief Allocates an instance of a class using the given Allocator
    /// @tparam ClassType The type of class
    /// @tparam Args The constructor args
    /// @param allocator The allocator to use
    /// @param args The args to pass to the constructor
    /// @return The created instance
    template<typename ClassType, typename ... Args>
    ClassType* New(Allocator& allocator, Args&& ... args)
    {
        auto* ptr = static_cast<ClassType*>(Memory::Allocate(allocator, sizeof(ClassType)));
        Construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    /// @brief Allocates an instance of a class using the default Allocator
    /// @tparam ClassType The type of class
    /// @tparam Args The constructor args
    /// @param args The args to pass to the constructor
    /// @return The created instance
    template<typename ClassType, typename ... Args>
    ClassType* DefaultNew(Args&& ... args)
    {
        auto* ptr = static_cast<ClassType*>(Memory::Allocate(*Allocator::GetDefaultAllocator(), sizeof(ClassType)));
        Construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    /// @brief Deletes an instance of a class created by the given Allocator.
    /// NOTE: The Allocator should be the same one used to create the instance
    /// @tparam ClassType The type of class
    /// @param allocator The allocator that allocated the instance
    /// @param ptr A pointer to the instance
    /// @param ptrSize The size of the pointer type. Useful if deleting a base type that's actually a derived type
    template<typename ClassType>
    void Delete(Allocator& allocator, ClassType* ptr, uint64 ptrSize = sizeof(ClassType)) noexcept
    {
        Destruct(ptr);
        Memory::Free(allocator, ptr, ptrSize);
    }

    /// @brief Deletes an instance of a class created by the default Allocator
    /// @tparam ClassType The type of class
    /// @param ptr A pointer to the instance
    /// @param ptrSize The size of the pointer type. Useful if deleting a base type that's actually a derived type
    template<typename ClassType>
    void DefaultDelete(ClassType* ptr, uint64 ptrSize = sizeof(ClassType)) noexcept
    {
        Destruct(ptr);
        Memory::Free(*Allocator::GetDefaultAllocator(), ptr, ptrSize);
    }
}

#endif //COCOENGINE_MEMORYOVERRIDES_H