//
// Created by cullen on 3/7/26.
//

#include "RTTI.h"

#include <string_view>

#include "Coco/Core/Types/String.h"

namespace Coco
{
    ClassRTTI::ClassRTTI(const char* typeName, const ClassRTTI* baseType) :
        TypeName(typeName),
        TypeID(ToHash(typeName)),
        BaseTypeRTTI(baseType)
    {
        GetRegistry().emplace(TypeID, this);
    }

    const ClassRTTI* ClassRTTI::TryGetClass(uint64 typeID)
    {
        const auto& registry = GetRegistry();
        if (registry.contains(typeID))
            return registry.at(typeID);

        return nullptr;
    }

    std::unordered_map<uint64, ClassRTTI*>& ClassRTTI::GetRegistry()
    {
        // TODO: don't use unordered_map and figure out static allocation for this
        static std::unordered_map<uint64, ClassRTTI*> registry;
        return registry;
    }

    bool operator==(const ClassRTTI& lhs, const ClassRTTI& rhs)
    {
        return lhs.TypeID == rhs.TypeID;
    }

    bool ClassRTTI::Is(uint64 typeID) const
    {
        if (typeID == TypeID)
            return true;

        if (!BaseTypeRTTI)
            return false;

        return BaseTypeRTTI->Is(typeID);
    }

    ClassRTTI RTTIObject::_rtti = ClassRTTI(RTTIObject::GetClassTypeName(), nullptr);
} // Coco