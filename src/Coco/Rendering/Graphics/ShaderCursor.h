//
// Created by cullen on 3/21/26.
//

#ifndef COCOENGINE_SHADERCURSOR_H
#define COCOENGINE_SHADERCURSOR_H
#include "ShaderBufferInterface.h"

#include "Coco/Core/Math/Vector2.h"
#include "Coco/Core/Math/Vector3.h"
#include "Coco/Core/Math/Vector4.h"
#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Types/Color.h"
#include "Coco/Rendering/ShaderTypes.h"
#include <Coco/Core/Memory/Ptrs.h>

namespace slang
{
    class TypeLayoutReflection;
}

namespace Coco
{
    class ShaderProgram;
    class Texture;

    struct ShaderElementLocation
    {
        uint64 ByteOffset;
        uint32 BindingRangeOffset;
        uint32 ArrayIndexInBindingRange;

        ShaderElementLocation();
        ShaderElementLocation(uint64 byteOffset, uint32 bindingRangeOffset, uint32 arrayIndexInBindingRange);
    };

    class ShaderCursor
    {
    public:
        ShaderCursor();
        ShaderCursor(ShaderBufferInterface* bufferInterface);
        ShaderCursor(slang::TypeLayoutReflection* typeLayout, const ShaderElementLocation& location, ShaderBufferInterface* bufferInterface);
        ~ShaderCursor();

        operator bool() const { return IsValid(); }

        void BindToInterface(ShaderBufferInterface& bufferInterface);

        ShaderCursor Field(const char* name) const;
        ShaderCursor Field(uint32 index) const;
        ShaderCursor ArrayElement(uint32 index) const;
        void Write(float value);
        void Write(const Vector2& value);
        void Write(const Vector3& value);
        void Write(const Vector4& value);
        void Write(int value);
        void Write(const Vector2i& value);
        void Write(const Vector3i& value);
        void Write(const Vector4i& value);
        void Write(uint32 value);
        void Write(Span<const uint32, 2> value);
        void Write(Span<const uint32, 3> value);
        void Write(Span<const uint32, 4> value);
        void Write(const Matrix4x4& value);
        void Write(const Color& value);
        void Write(SharedPtr<Texture> texture);

        bool IsValid() const { return _bufferInterface != nullptr; }

    private:
        slang::TypeLayoutReflection* _typeLayout;
        ShaderElementLocation _currentLocation;
        ShaderBufferInterface* _bufferInterface;
    };
} // Coco

#endif //COCOENGINE_SHADERCURSOR_H