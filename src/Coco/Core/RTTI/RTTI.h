//
// Created by cullen on 3/7/26.
//

#ifndef COCOENGINE_RTTI_H
#define COCOENGINE_RTTI_H
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Types/Map.h"

namespace Coco
{
    struct ClassRTTI
    {
        uint64 TypeID;
        const char* TypeName;
        const ClassRTTI* BaseTypeRTTI;

        ClassRTTI(const char* typeName, const ClassRTTI* baseTypeRTTI);

        static const ClassRTTI* TryGetClass(uint64 typeID);

        bool IsBaseType() const { return BaseTypeRTTI == nullptr; }
        bool Is(uint64 typeID) const;
        bool Is(const ClassRTTI& other) const { return Is(other.TypeID); }

        template<typename OtherType>
        bool Is() const
        {
            return Is(OtherType::GetTypeID());
        }

    private:
        static std::unordered_map<uint64, ClassRTTI*>& GetRegistry();
    };

    bool operator==(const ClassRTTI& lhs, const ClassRTTI& rhs);
    inline bool operator!=(const ClassRTTI& lhs, const ClassRTTI& rhs) { return !(lhs == rhs); }

    /// @brief Base class for all objects participating in RTTI.
    /// Each object needs to have DECLARE_RTTI_TYPE() somewhere in its class declaration and
    /// DEFINE_RTTI_BASETYPE() or DEFINE_RTTI_TYPE() somewhere
    class RTTIObject
    {
    public:
        virtual ~RTTIObject() = default;

        static const ClassRTTI& GetClassRTTI() { return _rtti; }
        static const char* GetClassTypeName() noexcept { return "RTTIObject"; }
        static uint64 GetClassTypeID() noexcept { return _rtti.TypeID; }

        virtual const ClassRTTI& GetRTTI() const { return _rtti; }

        const char* GetTypeName() const { return GetRTTI().TypeName; }
        uint64 GetTypeID() const { return GetRTTI().TypeID; }
        bool Is(uint64 typeID) const { return GetRTTI().Is(typeID); }

        template<typename OtherType>
        bool Is() const { return Is(OtherType::GetClassTypeID()); }

    private:
        static ClassRTTI _rtti;
    };
} // Coco

namespace std
{
    template<>
    struct hash<Coco::ClassRTTI>
    {
        size_t operator()(const Coco::ClassRTTI& type) const noexcept
        {
            return type.TypeID;
        }
    };
}

/// @brief Place this in a class to have it participate in the RTTI system. Make sure to DEFINE_RTTI_TYPE() somewhere for the class
#define DECLARE_RTTI_TYPE(Type) \
public: \
static const ClassRTTI& GetClassRTTI() { return _rtti; } \
static const char* GetClassTypeName() noexcept { return #Type; } \
static uint64 GetClassTypeID() noexcept { return _rtti.TypeID; } \
const ClassRTTI& GetRTTI() const override { return _rtti; } \
private: \
static ClassRTTI _rtti;

#define DEFINE_RTTI_BASETYPE(Type) \
ClassRTTI Type::_rtti = ClassRTTI(Type::GetClassTypeName(), &RTTIObject::GetClassRTTI())

#define DEFINE_RTTI_TYPE(Type, BaseType) \
ClassRTTI Type::_rtti = ClassRTTI(Type::GetClassTypeName(), &BaseType::GetClassRTTI())

#endif //COCOENGINE_RTTI_H