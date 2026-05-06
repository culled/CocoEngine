//
// Created by cullen on 3/7/26.
//

#include "Resource.h"

#include <limits>

namespace Coco
{
    DEFINE_RTTI_BASETYPE(Resource);

    Resource::Resource(Engine* engine, uint64 id) :
        _engine(engine),
        _id(id)
    {}
} // Coco