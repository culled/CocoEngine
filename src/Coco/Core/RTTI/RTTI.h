//
// Created by cullen on 3/7/26.
//

#ifndef COCOENGINE_RTTI_H
#define COCOENGINE_RTTI_H
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Types/Map.h"

namespace Coco
{
    /// @brief Information for a polymorphic class participating in the static RTTI system
    struct ClassRTTI
    {
        /// @brief The class's ID
        uint64 TypeID;

        /// @brief The class's name
        const char* TypeName;

        /// @brief Information for the class that this class is derived from
        const ClassRTTI* BaseTypeRTTI;

        ClassRTTI(const char* typeName, const ClassRTTI* baseTypeRTTI);

        /// @brief Attempts to get the type information for a given class via the class's ID
        /// @param typeID The ID of the class
        /// @return The class's type information, or nullptr if the type information does not exist
        static const ClassRTTI* TryGetClass(uint64 typeID);

        /// @brief Determines if this class is a base class and isn't derived from another class
        /// @return True if this class is not derived from any other classes
        bool IsBaseType() const { return BaseTypeRTTI == nullptr; }

        /// @brief Determines if this class is derived from a base class
        /// @param typeID The base class's ID
        /// @return True if this class is derived from the given class
        bool Is(uint64 typeID) const;

        /// @brief Determines if this class is derived from a base class
        /// @param other The base class type information
        /// @return True if this class is derived from the given class
        bool Is(const ClassRTTI& other) const { return Is(other.TypeID); }

        /// @brief Determines if this class is derived from a base class
        /// @tparam OtherType The base class type
        /// @return True if this class is derived from the given class
        template<typename OtherType>
        bool Is() const
        {
            return Is(OtherType::GetTypeID());
        }

    private:
        /// @brief Gets the static class registry
        /// @return The class type registry
        static std::unordered_map<uint64, ClassRTTI*>& GetRegistry();
    };

    bool operator==(const ClassRTTI& lhs, const ClassRTTI& rhs);
    inline bool operator!=(const ClassRTTI& lhs, const ClassRTTI& rhs) { return !(lhs == rhs); }

    /// @brief Base class for all classes participating in static RTTI.
    /// Each class needs to have DECLARE_RTTI_TYPE() somewhere in its class declaration and
    /// DEFINE_RTTI_BASETYPE() or DEFINE_RTTI_TYPE() somewhere in a cpp file
    class RTTIObject
    {
    public:
        virtual ~RTTIObject() = default;

        /// @brief Gets this class's type information
        /// @return The class's type information
        static const ClassRTTI& GetClassRTTI() { return _rtti; }

        /// @brief Gets this class's name
        /// @return The name of this class
        static const char* GetClassTypeName() noexcept { return "RTTIObject"; }

        /// @brief Gets this class's ID
        /// @return The ID of this class
        static uint64 GetClassTypeID() noexcept { return _rtti.TypeID; }

        /// @brief Gets the most-derived class's type information
        /// @return The class's type information
        virtual const ClassRTTI& GetRTTI() const { return _rtti; }

        /// @brief Gets the most-derived class's name
        /// @return The class's name
        const char* GetTypeName() const { return GetRTTI().TypeName; }

        /// @brief Gets the most-derived class's ID
        /// @return The class's ID
        uint64 GetTypeID() const { return GetRTTI().TypeID; }

        /// @brief Determines if this class is derived from a base class
        /// @param typeID The base class's ID
        /// @return True if this class is derived from the given class
        bool Is(uint64 typeID) const { return GetRTTI().Is(typeID); }

        /// @brief Determines if this class is derived from a base class
        /// @tparam OtherType The base class type
        /// @return True if this class is derived from the given class
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

/// @brief Place this in a class definition to have it participate in the RTTI system. Make sure to DEFINE_RTTI_TYPE() in a cpp file somewhere for the class
#define DECLARE_RTTI_TYPE(Type) \
public: \
static const ClassRTTI& GetClassRTTI() { return _rtti; } \
static const char* GetClassTypeName() noexcept { return #Type; } \
static uint64 GetClassTypeID() noexcept { return _rtti.TypeID; } \
const ClassRTTI& GetRTTI() const override { return _rtti; } \
private: \
static ClassRTTI _rtti;

/// @brief Place this in a cpp file somewhere to declare the given class as a base type. Base classes only derive from RTTIObject
#define DEFINE_RTTI_BASETYPE(Type) \
ClassRTTI Type::_rtti = ClassRTTI(Type::GetClassTypeName(), &RTTIObject::GetClassRTTI())

/// @brief Place this in a cpp file somewhere to declare the given class as deriving from another class
#define DEFINE_RTTI_TYPE(Type, ParentType) \
ClassRTTI Type::_rtti = ClassRTTI(Type::GetClassTypeName(), &ParentType::GetClassRTTI())

#endif //COCOENGINE_RTTI_H