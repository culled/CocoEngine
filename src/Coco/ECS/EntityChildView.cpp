//
// Created by cullen on 4/2/26.
//

#include "EntityChildView.h"

#include "ECSService.h"

namespace Coco
{
    EntityChildView::Iterator::Iterator(EntityChildView& view, bool end) :
        _view(&view),
        _childIndex(0)
    {
        if (end && _view->_entityData)
            _childIndex = view._entityData->ChildrenIDs.GetCount();

        UpdateCurrentChild();
    }

    EntityChildView::Iterator::Iterator(const Iterator& other) :
        _view(other._view),
        _childIndex(other._childIndex)
    {
        UpdateCurrentChild();
    }

    EntityChildView::Iterator& EntityChildView::Iterator::operator=(const Iterator& rhs)
    {
        _view = rhs._view;
        _childIndex = rhs._childIndex;

        UpdateCurrentChild();

        return *this;
    }

    EntityChildView::Iterator& EntityChildView::Iterator::operator++()
    {
        if (!_view->_entityData)
            return *this;

        ++_childIndex;

        if (_childIndex > _view->_entityData->ChildrenIDs.GetCount())
            _childIndex = _view->_entityData->ChildrenIDs.GetCount();

        UpdateCurrentChild();

        return *this;
    }

    EntityChildView::Iterator EntityChildView::Iterator::operator++(int)
    {
        Iterator copy = *this;
        ++*this;

        return copy;
    }

    EntityChildView::Iterator& EntityChildView::Iterator::operator--()
    {
        if (_childIndex == 0)
            return *this;

        --_childIndex;

        UpdateCurrentChild();

        return *this;
    }

    EntityChildView::Iterator EntityChildView::Iterator::operator--(int)
    {
        Iterator copy = *this;
        --*this;

        return copy;
    }

    bool EntityChildView::Iterator::operator==(const Iterator& other) const
    {
        return _view->_entityData == other._view->_entityData && _childIndex == other._childIndex;
    }

    void EntityChildView::Iterator::UpdateCurrentChild()
    {
        UUID entityID = UUID::Nil;

        if (_view->_entityData && _childIndex < _view->_entityData->ChildrenIDs.GetCount())
            entityID = _view->_entityData->ChildrenIDs[_childIndex];

        _currentChild = Entity(_view->_ecs, entityID);
    }

    EntityChildView::EntityChildView(Entity& entity) :
        _ecs(entity._ecs),
        _entityData(nullptr)
    {
        if (_ecs)
            _entityData = _ecs->GetEntityStorage()->TryGet(entity._id);
    }
} // Coco