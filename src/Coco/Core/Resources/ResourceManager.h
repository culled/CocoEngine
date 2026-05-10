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
    /// @brief A manager for resources used by the Engine
    class ResourceManager
    {
    public:
        ResourceManager(Engine* engine);

        /// @brief Creates a resource and returns it
        /// @tparam ResourceType The type of resource
        /// @tparam Args The constructor arguments
        /// @param name The unique name of the resource
        /// @param args The arguments to pass to the resource's constructor
        /// @return The created resource
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

        /// @brief Determines if a resource exists
        /// @param name The unique name of the resource
        /// @return True if the resource exists
        bool HasResource(const char* name) const noexcept { return HasResource(ToHash(name)); }

        /// @brief Determines if a resource exists
        /// @param id The resource's ID
        /// @return True if the resource exists
        bool HasResource(uint64 id) const noexcept;

        /// @brief Gets a resource. NOTE: make sure the resource exists by checking HasResource() first
        /// @param name The unique name of the resource
        /// @return The resource
        SharedPtr<Resource> GetResource(const char* name) { return GetResource(ToHash(name)); }

        /// @brief Gets a resource. NOTE: make sure the resource exists by checking HasResource() first
        /// @param id The resource's ID
        /// @return The resource
        SharedPtr<Resource> GetResource(uint64 id);

        /// @brief Gets a resource, casting it to the given type
        /// @tparam ResourceType The type of resource
        /// @param name The unique name of the resource
        /// @return The resource, or nullptr if a resource of the given type could not be found
        template<typename ResourceType>
        SharedPtr<ResourceType> GetResourceAs(const char* name)
        {
            uint64 id = ToHash(name);
            return GetResourceAs<ResourceType>(id);
        }

        /// @brief Gets a resource, casting it to the given type
        /// @tparam ResourceType The type of resource
        /// @param id The resource's ID
        /// @return The resource, or nullptr if a resource of the given type could not be found
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

        /// @brief Removes a resource. NOTE: this will only destroy the resource if no other objects hold a reference to it
        /// @param id The resource's ID
        void RemoveResource(uint64 id) noexcept;

    private:
        Engine* _engine;
        Map<uint64, SharedPtr<Resource>> _resources;
    };
} // Coco

#endif //COCOENGINE_RESOURCEMANAGER_H