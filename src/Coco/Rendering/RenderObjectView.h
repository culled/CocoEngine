//
// Created by cullen on 4/8/26.
//

#ifndef COCOENGINE_RENDEROBJECTVIEW_H
#define COCOENGINE_RENDEROBJECTVIEW_H

#include "Coco/Core/Types/CoreTypes.h"
#include "RenderSceneTypes.h"

namespace Coco
{
    class RenderScene;

    /// @brief A view over RenderObjects in a RenderScene
    class RenderObjectView
    {
    public:
        class Iterator
        {
        public:
            using value_type = const RenderObject;
            using pointer = const RenderObject*;
            using reference = const RenderObject&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            Iterator(const RenderObjectView& view, bool isEnd);
            Iterator(const Iterator& other);

            friend void swap(Iterator& a, Iterator& b) noexcept;

            Iterator& operator=(Iterator rhs);

            pointer operator->();
            reference operator*();

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
            const RenderObjectView* _view;
            uint64 _lastIndex;
            uint64 _currentIndex;
        };

        RenderObjectView(const RenderScene& scene);

        Iterator begin() const { return Iterator(*this, false); }
        Iterator end() const { return Iterator(*this, true); }

    private:
        const RenderScene* _scene;
    };
} // Coco

#endif //COCOENGINE_RENDEROBJECTVIEW_H