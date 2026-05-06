//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_ENUMTYPES_H
#define COCOENGINE_ENUMTYPES_H

/// @brief Defines bitwise operators for enum flags
#define EnumFlagOperators(FlagType)																															\
constexpr FlagType operator|(const FlagType& a, const FlagType& b) noexcept { return static_cast<FlagType>(static_cast<int>(a) | static_cast<int>(b)); }	\
constexpr FlagType operator&(const FlagType& a, const FlagType& b) noexcept { return static_cast<FlagType>(static_cast<int>(a) & static_cast<int>(b)); }	\
constexpr FlagType& operator|=(FlagType& a, const FlagType& b) noexcept { a = a | b; return a; }															\
constexpr FlagType& operator&=(FlagType& a, const FlagType& b) noexcept { a = a & b; return a; }															\
constexpr FlagType operator~(const FlagType& v) noexcept { return static_cast<FlagType>(~static_cast<int>(v)); }

#endif //COCOENGINE_ENUMTYPES_H