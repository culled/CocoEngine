//
// Created by cullen on 3/7/26.
//

#ifndef COCOENGINE_RESOURCE_H
#define COCOENGINE_RESOURCE_H
#include <limits>

#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/RTTI/RTTI.h"

namespace Coco
{
    class Engine;

    /// @brief Base class for all resources used by the Engine. Resources can be saved and loaded from disk by the ResourceManager
    class Resource : public RTTIObject
    {
        DECLARE_RTTI_TYPE(Resource)

    public:
        /// @brief An invalid resource ID
        static constexpr uint64 InvalidID = std::numeric_limits<uint64>::max();

        virtual ~Resource() = default;

        /// @brief Gets this resource's ID
        /// @return This resource's ID
        uint64 GetID() const { return _id; }

    protected:
        Engine* _engine;
        uint64 _id;

        Resource(Engine* engine, uint64 id);
    };
} // Coco

#endif //COCOENGINE_RESOURCE_H