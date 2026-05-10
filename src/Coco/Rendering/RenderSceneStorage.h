//
// Created by cullen on 3/21/26.
//

#ifndef COCOENGINE_RENDERSCENESTORAGE_H
#define COCOENGINE_RENDERSCENESTORAGE_H

#include <Coco/Core/Types/CoreTypes.h>

#include "Coco/Core/Math/Math.h"
#include "Coco/Core/Memory/Allocators/LinearAllocator.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Core/Types/PagedArray.h"

#include "Graphics/ShaderUniformValue.h"

namespace Coco
{
    /// @brief A group of shader uniforms
    struct ShaderUniformGroup
    {
        Span<const ShaderUniformValue> Uniforms;

        ShaderUniformGroup(Span<const ShaderUniformValue> uniforms);
    };

    /// @brief Data storage for a RenderScene
    class RenderSceneStorage
    {
    public:
        RenderSceneStorage(Allocator* allocator, uint64 rawDataPageSize, uint64 uniformPageSize);
        ~RenderSceneStorage();

        /// @brief Stores arbitrary render data. The same ID can be used for different data types
        /// @tparam DataType The type of data
        /// @param id The ID of the data
        /// @param data The data
        template<typename DataType>
        void Store(uint64 id, const DataType& data)
        {
            uint64 key = GetKey<DataType>(id);
            if (_dataMap.Contains(key))
                return;

            uint64 size = sizeof(DataType);
            LinearAllocator* targetAllocator = nullptr;
            for (auto& allocator : _allocators)
            {
                if (allocator.GetRemainingSpace() >= size)
                {
                    targetAllocator = &allocator;
                    break;
                }
            }

            if (!targetAllocator)
            {
                targetAllocator = &_allocators.EmplaceBack(_allocator->GetGroup(), _pageSize, _allocator);
            }

            void* memory = targetAllocator->Allocate(size);
            COCO_ASSERT(memory, "Memory could not be allocated");

            memcpy(memory, &data, sizeof(DataType));

            _dataMap.Emplace(key, memory);
        }

        /// @brief Determines if render data with the given ID and type exists
        /// @tparam DataType The type of data
        /// @param id The ID of the data
        /// @return True if the data exists
        template<typename DataType>
        bool Has(uint64 id) const
        {
            uint64 key = GetKey<DataType>(id);
            return _dataMap.Contains(key);
        }

        /// @brief Gets arbitrary render data previously stored
        /// @tparam DataType The type of data
        /// @param id The ID of the data
        /// @return The data
        template<typename DataType>
        const DataType* Get(uint64 id) const
        {
            uint64 key = GetKey<DataType>(id);
            return static_cast<const DataType*>(_dataMap.Get(key));
        }

        /// @brief Stores a group of shader uniforms with a given ID. This ID is separate from the ID of data stored using Store()
        /// @param id The ID of the data
        /// @param uniforms The uniforms
        void StoreUniforms(uint64 id, Span<const ShaderUniformValue> uniforms);

        /// @brief Determines if a group of shader uniforms with the given ID exist
        /// @param id The ID of the data
        /// @return True if shader uniforms with the given ID exist
        bool HasUniforms(uint64 id) const;

        /// @brief Gets a group of shader uniforms previously stored
        /// @param id The ID of the data
        /// @return The group of shader uniforms
        Span<const ShaderUniformValue> GetUniforms(uint64 id) const;

        /// @brief Clears all stored data
        void Clear();

    private:
        Allocator* _allocator;
        uint64 _pageSize;
        Array<LinearAllocator> _allocators;
        Map<uint64, void*> _dataMap;
        PagedArray<ShaderUniformValue> _shaderUniformValues;
        Map<uint64, ShaderUniformGroup> _shaderUniformGroups;

        /// @brief Creates a key unique to a combination of data type and ID
        /// @tparam DataType The type of data
        /// @param id The ID of the data
        /// @return The data key
        template<typename DataType>
        static uint64 GetKey(uint64 id)
        {
            return Math::CombineHashes(typeid(DataType).hash_code(), id);
        }
    };
} // Coco

#endif //COCOENGINE_RENDERSCENESTORAGE_H