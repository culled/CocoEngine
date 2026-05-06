//
// Created by cullen on 4/8/26.
//

#include "RenderObjectView.h"

#include "RenderScene.h"

namespace Coco
{
    RenderObjectView::Iterator::Iterator(const RenderObjectView& view, bool isEnd) :
        _view(&view),
        _lastIndex(view._scene->_firstRenderObjectIndex + view._scene->_renderObjectCount),
        _currentIndex(isEnd ? _lastIndex : view._scene->_firstRenderObjectIndex)
    {}

    RenderObjectView::Iterator::Iterator(const Iterator& other) :
        _view(other._view),
        _lastIndex(other._lastIndex),
        _currentIndex(other._currentIndex)
    {}

    RenderObjectView::Iterator& RenderObjectView::Iterator::operator=(Iterator rhs)
    {
        swap(*this, rhs);
        return *this;
    }

    RenderObjectView::Iterator::pointer RenderObjectView::Iterator::operator->()
    {
        return &_view->_scene->_frame->_renderObjects[_currentIndex];
    }

    RenderObjectView::Iterator::reference RenderObjectView::Iterator::operator*()
    {
        return _view->_scene->_frame->_renderObjects[_currentIndex];
    }

    RenderObjectView::Iterator& RenderObjectView::Iterator::operator++()
    {
        if (_currentIndex < _lastIndex)
            _currentIndex++;

        return *this;
    }

    RenderObjectView::Iterator RenderObjectView::Iterator::operator++(int)
    {
        Iterator copy(*this);
        ++*this;
        return copy;
    }

    RenderObjectView::Iterator& RenderObjectView::Iterator::operator--()
    {
        if (_currentIndex > _view->_scene->_firstRenderObjectIndex)
            _currentIndex--;

        return *this;
    }

    RenderObjectView::Iterator RenderObjectView::Iterator::operator--(int)
    {
        Iterator copy(*this);
        --*this;
        return copy;
    }

    bool RenderObjectView::Iterator::operator==(const Iterator& other) const
    {
        return _view == other._view && _currentIndex == other._currentIndex;
    }

    RenderObjectView::RenderObjectView(const RenderScene& scene) :
        _scene(&scene)
    {}

    void swap(RenderObjectView::Iterator& a, RenderObjectView::Iterator& b) noexcept
    {
        using std::swap;
        swap(a._view, b._view);
        swap(a._lastIndex, b._lastIndex);
        swap(a._currentIndex, b._currentIndex);
    }
} // Coco