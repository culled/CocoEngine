//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_MAP_H
#define COCOENGINE_MAP_H
#include <functional>
#include <unordered_map>

#include "Coco/Core/Memory/Allocator.h"

namespace Coco
{
    template<typename KeyType, typename ValueType>
    class Map
    {
    public:
        using Iterator = std::unordered_map<KeyType, ValueType>::iterator;
        using ConstIterator = std::unordered_map<KeyType, ValueType>::const_iterator;

    public:
        Map(Allocator* allocator = nullptr) :
            _allocator(allocator ? allocator : Allocator::GetDefaultAllocator()),
            _map() // TODO: use allocator
        {}

        ~Map() noexcept
        {
            _map.clear();
        }

        ValueType& Add(const KeyType& key, const ValueType& value)
        {
            auto result = _map.emplace(key, value);
            return result.first->second;
        }

        template<typename ... Args>
        ValueType& Emplace(const KeyType& key, Args&& ... args)
        {
            auto result = _map.try_emplace(key, std::forward<Args>(args)...);
            return result.first->second;
        }

        bool Contains(const KeyType& key) const noexcept
        {
            return _map.contains(key);
        }

        ValueType& operator[](const KeyType& key)
        {
            return _map[key];
        }

        const ValueType& operator[](const KeyType& key) const
        {
            return _map[key];
        }

        ValueType& Get(const KeyType& key)
        {
            return _map.at(key);
        }

        const ValueType& Get(const KeyType& key) const
        {
            return _map.at(key);
        }

        void Remove(const KeyType& key) noexcept
        {
            _map.erase(key);
        }

        void Clear() noexcept
        {
            _map.clear();
        }

        ValueType* TryGetValue(const KeyType& key)
        {
            if (!Contains(key))
                return nullptr;

            return &Get(key);
        }

        const ValueType* TryGetValue(const KeyType& key) const
        {
            if (!Contains(key))
                return nullptr;

            return &Get(key);
        }

        bool IsEmpty() const noexcept
        {
            return _map.empty();
        }

        uint64 GetCount() const noexcept { return _map.size(); }

        Iterator begin() { return _map.begin(); }
        Iterator end() { return _map.end(); }

        ConstIterator begin() const { return _map.begin(); }
        ConstIterator end() const { return _map.end(); }

    private:
        Allocator* _allocator;
        std::unordered_map<KeyType, ValueType, std::hash<KeyType>> _map;
    };
}
#endif //COCOENGINE_MAP_H