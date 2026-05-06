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

    class Resource : public RTTIObject
    {
        DECLARE_RTTI_TYPE(Resource)

    public:
        static constexpr uint64 InvalidID = std::numeric_limits<uint64>::max();

    public:
        virtual ~Resource() = default;

        uint64 GetID() const { return _id; }

    protected:
        Engine* _engine;
        uint64 _id;

    protected:
        Resource(Engine* engine, uint64 id);
    };
} // Coco

#endif //COCOENGINE_RESOURCE_H