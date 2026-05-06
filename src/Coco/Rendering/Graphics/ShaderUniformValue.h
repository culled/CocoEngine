//
// Created by cullen on 3/11/26.
//

#ifndef COCOENGINE_SHADERUNIFORMVALUE_H
#define COCOENGINE_SHADERUNIFORMVALUE_H

#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Math/Vector2.h"
#include "Coco/Core/Math/Vector3.h"
#include "Coco/Core/Math/Vector4.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Rendering/Texture.h"
#include "Coco/Rendering/RenderGraph/RenderGraphTypes.h"
#include <variant>

namespace slang
{
    struct VariableLayoutReflection;
}

namespace Coco
{
    class ShaderCursor;

    /// @brief Shader uniform types
    enum class ShaderUniformType : uint8
    {
        Float = 0,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        UInt,
        UInt2,
        UInt3,
        UInt4,
        Bool,
        Matrix4x4,
        Texture
    };

    /*enum class ShaderUniformScalarType : uint8
    {
        Float = 0,
        Int,
        UInt,
        Bool,
        Texture
    };*/

    class ShaderUniformValue
    {
    public:
        ShaderUniformValue();
        ShaderUniformValue(const char* name, float value);
        ShaderUniformValue(const char* name, const Vector2& value);
        ShaderUniformValue(const char* name, const Vector3& value);
        ShaderUniformValue(const char* name, const Vector4& value);
        ShaderUniformValue(const char* name, int value);
        ShaderUniformValue(const char* name, const Vector2i& value);
        ShaderUniformValue(const char* name, const Vector3i& value);
        ShaderUniformValue(const char* name, const Vector4i& value);
        ShaderUniformValue(const char* name, uint32 value);
        ShaderUniformValue(const char* name, Span<const uint32, 2> value);
        ShaderUniformValue(const char* name, Span<const uint32, 3> value);
        ShaderUniformValue(const char* name, Span<const uint32, 4> value);
        ShaderUniformValue(const char* name, bool value);
        ShaderUniformValue(const char* name, const Color& value);
        ShaderUniformValue(const char* name, const Matrix4x4& value);
        ShaderUniformValue(const char* name, SharedPtr<Texture> value);

        static bool TryCreateFromField(slang::VariableLayoutReflection* field, ShaderUniformValue& outUniform);

        const String& GetName() const { return _name; }
        ShaderUniformType GetUniformType() const { return _type; }

        void SetValue(float value);
        float AsFloat() const;

        void SetValue(const Vector2& value);
        Vector2 AsFloat2() const;

        void SetValue(const Vector3& value);
        Vector3 AsFloat3() const;

        void SetValue(const Vector4& value);
        Vector4 AsFloat4() const;

        void SetValue(int value);
        int AsInt() const;

        void SetValue(const Vector2i& value);
        Vector2i AsInt2() const;

        void SetValue(const Vector3i& value);
        Vector3i AsInt3() const;

        void SetValue(const Vector4i& value);
        Vector4i AsInt4() const;

        void SetValue(uint32 value);
        uint32 AsUInt() const;

        void SetValue(Span<const uint32, 2> value);
        Span<const uint32, 2> AsUInt2() const;

        void SetValue(Span<const uint32, 3> value);
        Span<const uint32, 3> AsUInt3() const;

        void SetValue(Span<const uint32, 4> value);
        Span<const uint32, 4> AsUInt4() const;

        void SetValue(bool value);
        bool AsBool() const;

        void SetValue(const Color& color);
        Color AsColor() const;

        void SetValue(const Matrix4x4& value);
        Matrix4x4 AsMatrix4x4() const;

        void SetValue(const SharedPtr<Texture>& value);
        SharedPtr<Texture> AsTexture() const;

        void WriteInto(const ShaderCursor& cursor) const;

    private:
        String _name;
        ShaderUniformType _type;
        //ShaderUniformScalarType _scalarType;
        //uint8 _columnCount;
        //uint8 _rowCount;
        std::variant<
            float, Vector2, Vector3, Vector4,
            int, Vector2i, Vector3i, Vector4i,
            uint32, bool,
            Matrix4x4,
            SharedPtr<Texture>> _value;

        /*union
        {
            uint8 _raw[64];
            float _floatValue;
            Vector2 _float2Value;
            Vector3 _float3Value;
            Vector4 _float4Value;
            int _intValue;
            Vector2i _int2Value;
            Vector3i _int3Value;
            Vector4i _int4Value;
            uint32 _uintValue;
            uint32 _uint2Value[2];
            uint32 _uint3Value[3];
            uint32 _uint4Value[4];
            bool _boolValue;
            Matrix4x4 _matrix4x4Value;
            SharedPtr<Texture> _textureValue;
        };*/
    };
} // Coco

#endif //COCOENGINE_SHADERUNIFORMVALUE_H