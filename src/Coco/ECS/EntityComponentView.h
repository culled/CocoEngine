//
// Created by cullen on 4/5/26.
//

#ifndef COCOENGINE_ENTITYCOMPONENTVIEW_H
#define COCOENGINE_ENTITYCOMPONENTVIEW_H
#include "Entity.h"

#include "Coco/Core/Types/StackArray.h"
#include "ECSService.h"

namespace Coco
{
    class ECSService;

    template<typename FirstComponent, typename ... AdditionalComponents>
    class EntityComponentView
    {
    public:
        using RegistryIt = Map<UUID, FirstComponent>::Iterator;

        class Iterator
        {
        public:
            using value_type = Entity;
            using pointer = Entity*;
            using reference = Entity&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            Iterator(EntityComponentView& view, bool end) :
                _view(&view),
                _baseRegistry(nullptr),
                _currentEntity()
            {
                if (!_view->_ecs)
                    return;

                if (!_view->_componentRegistries.IsEmpty())
                {
                    _baseRegistry = static_cast<EntityComponentRegistry<FirstComponent>*>(_view->_componentRegistries.Front());
                    if (!_baseRegistry)
                        return;

                    _it = end ? _baseRegistry->_components.end() : _baseRegistry->_components.begin();

                    if (!end)
                        FindNextEntity(true);
                }
            }

            Iterator(const Iterator& other) :
                _view(other._view),
                _baseRegistry(other._baseRegistry),
                _it(other._it),
                _currentEntity(other._currentEntity)
            {}

            bool operator==(const Iterator& rhs) const noexcept
            {
                return _view == rhs._view && _currentEntity == rhs._currentEntity;
            }

            bool operator !=(const Iterator& rhs) const noexcept { return !(*this == rhs); }

            pointer operator->() { return &_currentEntity; }

            reference operator*() { return _currentEntity; }

            Iterator& operator++()
            {
                FindNextEntity(false);
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator copy(*this);
                FindNextEntity(false);
                return copy;
            }

        private:
            EntityComponentView* _view;
            EntityComponentRegistry<FirstComponent>* _baseRegistry;
            RegistryIt _it;
            Entity _currentEntity;

            void FindNextEntity(bool includeCurrent)
            {
                if (!_baseRegistry)
                    return;

                UUID currentID = UUID::Nil;

                if (!includeCurrent && _it != _baseRegistry->_components.end())
                    ++_it;

                while (_it != _baseRegistry->_components.end())
                {
                    UUID candidateID = _it->first;

                    if ((_view->_currentScene && _view->_ecs->GetEntityScene(candidateID) != _view->_currentScene) ||
                        (!_view->_ecs->IsEntityActiveInScene(candidateID) && _view->_onlyActiveEntities))
                    {
                        ++_it;
                        continue;
                    }

                    bool hasAll = true;

                    for (auto& registry : _view->_componentRegistries)
                    {
                        if (!registry || !registry->Has(candidateID))
                        {
                            hasAll = false;
                            break;
                        }
                    }

                    if (hasAll)
                    {
                        currentID = candidateID;
                        break;
                    }

                    ++_it;
                }

                _currentEntity = Entity(_view->_ecs, currentID);
            }
        };

    public:
        EntityComponentView(ECSService* ecs, bool onlyActiveEntities, Scene* currentScene) :
            _ecs(ecs),
            _onlyActiveEntities(onlyActiveEntities),
            _currentScene(currentScene),
            _componentRegistries()
        {
            if (!_ecs)
                return;

            //StackArray<ClassRTTI*, sizeof...(AdditionalComponents) + 1> componentTypes = { &FirstComponent::GetClassRTTI(), &AdditionalComponents::GetClassRTTI()... };

            const ClassRTTI* componentTypes[] = { &FirstComponent::GetClassRTTI(), &AdditionalComponents::GetClassRTTI()... };

            auto componentStorage = _ecs->GetComponentStorage();
            for (const ClassRTTI* type : componentTypes)
            {
                for (auto& pair : componentStorage->_registries)
                {
                    if (pair.first.Is(*type))
                    {
                        _componentRegistries.Append(pair.second.get());
                        break;
                    }
                }
            }
            /*for (auto& type : componentTypes)
            {
                if (auto exactTypeRegistry = componentStorage->_registries.TryGetValue(type))
                {
                    _componentRegistries.Append(exactTypeRegistry.get());
                    continue;
                }

                for (auto& pair : componentStorage->_registries)
                {
                    if (type.Is(pair.first))
                    {
                        _componentRegistries.Append(pair.second.get());
                        continue;
                    }
                }

                _componentRegistries.Append(nullptr);
            }*/
        }

        Iterator begin() { return Iterator(*this, false); }
        Iterator end() { return Iterator(*this, true); }

        Iterator begin() const { return Iterator(*this, false); }
        Iterator end() const { return Iterator(*this, true); }

    private:
        ECSService* _ecs;
        bool _onlyActiveEntities;
        Scene* _currentScene;
        StackArray<BaseEntityComponentRegistry*, 16> _componentRegistries;
    };
} // Coco

#endif //COCOENGINE_ENTITYCOMPONENTVIEW_H