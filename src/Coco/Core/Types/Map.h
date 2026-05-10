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
    /// @brief A relational container that maps keys to values in an indeterminate order
    /// @tparam KeyType The type of key
    /// @tparam ValueType The type of value
    template<typename KeyType, typename ValueType>
    class Map
    {
    public:
        using Iterator = std::unordered_map<KeyType, ValueType>::iterator;
        using ConstIterator = std::unordered_map<KeyType, ValueType>::const_iterator;

        Map(Allocator* allocator = nullptr) :
            _allocator(allocator ? allocator : Allocator::GetDefaultAllocator()),
            _map() // TODO: use allocator
        {}

        ~Map() noexcept
        {
            _map.clear();
        }

        /// @brief Adds a key-value pair. The value will be copy-constructed
        /// @param key The key
        /// @param value The value
        /// @return The added value
        ValueType& Add(const KeyType& key, const ValueType& value)
        {
            auto result = _map.emplace(key, value);
            return result.first->second;
        }

        /// @brief Constructs a key-value pair in-place
        /// @tparam Args Arguments to pass to the value's constructor
        /// @param key The key
        /// @param args Arguments to pass to the value's constructor
        /// @return The constructed value
        template<typename ... Args>
        ValueType& Emplace(const KeyType& key, Args&& ... args)
        {
            auto result = _map.try_emplace(key, std::forward<Args>(args)...);
            return result.first->second;
        }

        /// @brief Determines if a value for the given key exists
        /// @param key The key
        /// @return True if a value for the given key exists
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

        /// @brief Gets a value for a key. NOTE: ensure that the value exists first by using Contains()
        /// @param key The key
        /// @return The value
        ValueType& Get(const KeyType& key)
        {
            return _map.at(key);
        }

        /// @brief Gets a value for a key. NOTE: ensure that the value exists first by using Contains()
        /// @param key The key
        /// @return The value
        const ValueType& Get(const KeyType& key) const
        {
            return _map.at(key);
        }

        /// @brief Removes a key-value pair
        /// @param key The key
        void Remove(const KeyType& key) noexcept
        {
            _map.erase(key);
        }

        /// @brief Removes all key-value pairs
        void Clear() noexcept
        {
            _map.clear();
        }

        /// @brief Attempts to get a value for the given key
        /// @param key The key
        /// @return A pointer to the value, or nullptr if it does not exist
        ValueType* TryGetValue(const KeyType& key)
        {
            if (!Contains(key))
                return nullptr;

            return &Get(key);
        }

        /// @brief Attempts to get a value for the given key
        /// @param key The key
        /// @return A pointer to the value, or nullptr if it does not exist
        const ValueType* TryGetValue(const KeyType& key) const
        {
            if (!Contains(key))
                return nullptr;

            return &Get(key);
        }

        /// @brief Determines if this map is empty
        /// @return True if this map contains no key-value pairs
        bool IsEmpty() const noexcept
        {
            return _map.empty();
        }

        /// @brief Gets the number of stored key-value pairs
        /// @return The number of stored key-value pairs
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