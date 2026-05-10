//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_ENTITYCHILDVIEW_H
#define COCOENGINE_ENTITYCHILDVIEW_H
#include "EntityStorage.h"
#include "Entity.h"

namespace Coco
{
    class ECSService;

    /// @brief A view over the children of an Entity
    class EntityChildView
    {
    public:
        class Iterator
        {
        public:
            using value_type = Entity;
            using pointer = Entity*;
            using reference = Entity&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            Iterator(EntityChildView& view, bool end);
            Iterator(const Iterator& other);

            Iterator& operator=(const Iterator& rhs);

            pointer operator->() { return &_currentChild; }

            reference operator*() { return _currentChild; }

            Iterator& operator++();
            Iterator operator++(int);

            Iterator& operator--();
            Iterator operator--(int);

            bool operator==(const Iterator& other) const;

            bool operator!=(const Iterator& other) const
            {
                return !(*this == other);
            }

        private:
            EntityChildView* _view;
            Entity _currentChild;
            uint64 _childIndex;

            /// @brief Updates the current child entity
            void UpdateCurrentChild();
        };

        EntityChildView(Entity& entity);

        Iterator begin() { return Iterator(*this, false); }
        Iterator end() { return Iterator(*this, true); }

    private:
        ECSService* _ecs;
        const EntityData* _entityData;
    };
} // Coco

#endif //COCOENGINE_ENTITYCHILDVIEW_H