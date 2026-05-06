//
// Created by cullen on 3/7/26.
//

#ifndef COCOENGINE_RESOURCEMANAGER_H
#define COCOENGINE_RESOURCEMANAGER_H
#include "Resource.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Core/Types/String.h"

namespace Coco
{
    class ResourceManager
    {
    public:
        ResourceManager(Engine* engine);

        template<typename ResourceType, typename ... Args>
        SharedPtr<ResourceType> CreateResource(const char* name, Args&& ... args)
        {
            if (HasResource(name))
                return GetResourceAs<ResourceType>(name);

            uint64 id = ToHash(name);
            SharedPtr<ResourceType> res = CreateDefaultShared<ResourceType>(_engine, id, std::forward<Args>(args)...);
            _resources.Emplace(id, res);

            return res;
        }

        bool HasResource(const char* name) const noexcept { return HasResource(ToHash(name)); }
        bool HasResource(uint64 id) const noexcept;
        SharedPtr<Resource> GetResource(const char* name) { return GetResource(ToHash(name)); }
        SharedPtr<Resource> GetResource(uint64 id);

        template<typename ResourceType>
        SharedPtr<ResourceType> GetResourceAs(const char* name)
        {
            uint64 id = ToHash(name);
            return GetResourceAs<ResourceType>(id);
        }

        template<typename ResourceType>
        SharedPtr<ResourceType> GetResourceAs(uint64 id)
        {
            SharedPtr<Resource> res = GetResource(id);
            if (!res)
                return nullptr;

            uint64 resourceType = res->GetTypeID();
            if (resourceType != ResourceType::GetClassTypeID())
            {
                //COCO_ENGINE_LOG_ERROR("Resource %u is not type \"%s\"", id, ResourceType::GetClassTypeName());
                return nullptr;
            }

            return std::static_pointer_cast<ResourceType>(res);
        }

        void RemoveResource(uint64 id) noexcept;

    private:
        Engine* _engine;
        Map<uint64, SharedPtr<Resource>> _resources;
    };
} // Coco

#endif //COCOENGINE_RESOURCEMANAGER_H