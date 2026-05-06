//
// Created by cullen on 3/11/26.
//

#include "ShaderUniformValue.h"
#include "ShaderCursor.h"
#include <slang.h>

namespace Coco
{
    ShaderUniformValue::ShaderUniformValue() :
        _name(),
        _type(ShaderUniformType::Float),
        _value()
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, float value) :
        _name(name),
        _type(ShaderUniformType::Float),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Vector2& value) :
        _name(name),
        _type(ShaderUniformType::Float2),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Vector3& value) :
        _name(name),
        _type(ShaderUniformType::Float3),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Vector4& value) :
        _name(name),
        _type(ShaderUniformType::Float4),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, int value) :
        _name(name),
        _type(ShaderUniformType::Int),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Vector2i& value) :
        _name(name),
        _type(ShaderUniformType::Int2),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Vector3i& value) :
        _name(name),
        _type(ShaderUniformType::Int3),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Vector4i& value) :
        _name(name),
        _type(ShaderUniformType::Int4),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, uint32 value) :
        _name(name),
        _type(ShaderUniformType::UInt),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, Span<const uint32, 2> value) :
        _name(name),
        _type(ShaderUniformType::UInt2),
        _value(Vector2i(static_cast<int>(value[0]), static_cast<int>(value[1])))
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, Span<const uint32, 3> value) :
        _name(name),
        _type(ShaderUniformType::UInt3),
        _value(Vector3i(static_cast<int>(value[0]), static_cast<int>(value[1]), static_cast<int>(value[2])))
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, Span<const uint32, 4> value) :
        _name(name),
        _type(ShaderUniformType::UInt4),
        _value(Vector4i(static_cast<int>(value[0]), static_cast<int>(value[1]), static_cast<int>(value[2]), static_cast<int>(value[3])))
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, bool value) :
        _name(name),
        _type(ShaderUniformType::Bool),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Color& value) :
        ShaderUniformValue(name, value.AsVector4(false))
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, const Matrix4x4& value) :
        _name(name),
        _type(ShaderUniformType::Matrix4x4),
        _value(value)
    {}

    ShaderUniformValue::ShaderUniformValue(const char* name, SharedPtr<Texture> value) :
        _name(name),
        _type(ShaderUniformType::Texture),
        _value(value)
    {}

    bool ShaderUniformValue::TryCreateFromField(slang::VariableLayoutReflection* field, ShaderUniformValue& outUniform)
    {
        auto fieldLayout = field->getTypeLayout();
        auto kind = fieldLayout->getKind();
        auto columns = fieldLayout->getColumnCount();
        auto rows = fieldLayout->getRowCount();

        bool isValid = true;

        switch (kind)
        {
            case slang::TypeReflection::Kind::Scalar:
            case slang::TypeReflection::Kind::Vector:
            case slang::TypeReflection::Kind::Matrix:
            {
                auto type = fieldLayout->getScalarType();
                switch (type)
                {
                    case slang::TypeReflection::ScalarType::Float32:
                    {
                        switch (columns)
                        {
                            case 1:
                                outUniform.SetValue(0.0f);
                                break;
                            case 2:
                                outUniform.SetValue(Vector2::Zero);
                                break;
                            case 3:
                                outUniform.SetValue(Vector3::Zero);
                                break;
                            case 4:
                                switch (rows)
                                {
                                    case 1:
                                        outUniform.SetValue(Vector4::Zero);
                                        break;
                                    case 4:
                                        outUniform.SetValue(Matrix4x4());
                                        break;
                                    default:
                                        isValid = false;
                                        break;
                                }
                                break;
                            default:
                                isValid = false;
                                break;
                        }
                        break;
                    }
                    case slang::TypeReflection::ScalarType::Int32:
                    {
                        switch (columns)
                        {
                            case 1:
                                outUniform.SetValue(0);
                                break;
                            case 2:
                                outUniform.SetValue(Vector2i::Zero);
                                break;
                            case 3:
                                outUniform.SetValue(Vector3i::Zero);
                                break;
                            case 4:
                                outUniform.SetValue(Vector4i::Zero);
                                break;
                            default:
                                isValid = false;
                                break;
                        }
                        break;
                    }
                    case slang::TypeReflection::ScalarType::UInt32:
                    {
                        switch (columns)
                        {
                            case 1:
                                outUniform.SetValue(static_cast<uint32>(0));
                                break;
                            case 2:
                                outUniform.SetValue(Vector2i::Zero);
                                break;
                            case 3:
                                outUniform.SetValue(Vector3i::Zero);
                                break;
                            case 4:
                                outUniform.SetValue(Vector4i::Zero);
                                break;
                            default:
                                isValid = false;
                                break;
                        }
                        break;
                    }
                    default:
                        isValid = false;
                        break;
                }
                break;
            }
            case slang::TypeReflection::Kind::Resource:
            {
                auto shape = fieldLayout->getResourceShape();
                switch (shape & SLANG_RESOURCE_BASE_SHAPE_MASK)
                {
                    case SLANG_TEXTURE_2D:
                        outUniform.SetValue(SharedPtr<Texture>());
                        break;
                    default:
                        isValid = false;
                        break;
                }

                break;
            }
            default:
                isValid = false;
                break;
        }

        if (!isValid)
            return false;

        outUniform._name = field->getName();
        return true;
    }

    void ShaderUniformValue::SetValue(float value)
    {
        _type = ShaderUniformType::Float;
        _value = value;
    }

    float ShaderUniformValue::AsFloat() const
    {
        if (_type != ShaderUniformType::Float)
            throw Exception("Uniform value is not a Float");

        return std::get<float>(_value);
    }

    void ShaderUniformValue::SetValue(const Vector2& value)
    {
        _type = ShaderUniformType::Float2;
        _value = value;
    }

    Vector2 ShaderUniformValue::AsFloat2() const
    {
        if (_type != ShaderUniformType::Float2)
            throw Exception("Uniform value is not a Float2");

        return std::get<Vector2>(_value);
    }

    void ShaderUniformValue::SetValue(const Vector3& value)
    {
        _type = ShaderUniformType::Float3;
        _value = value;
    }

    Vector3 ShaderUniformValue::AsFloat3() const
    {
        if (_type != ShaderUniformType::Float3)
            throw Exception("Uniform value is not a Float3");

        return std::get<Vector3>(_value);
    }

    void ShaderUniformValue::SetValue(const Vector4& value)
    {
        _type = ShaderUniformType::Float4;
        _value = value;
    }

    Vector4 ShaderUniformValue::AsFloat4() const
    {
        if (_type != ShaderUniformType::Float4)
            throw Exception("Uniform value is not a Float4");

        return std::get<Vector4>(_value);
    }

    void ShaderUniformValue::SetValue(int value)
    {
        _type = ShaderUniformType::Int;
        _value = value;
    }

    int ShaderUniformValue::AsInt() const
    {
        if (_type != ShaderUniformType::Int)
            throw Exception("Uniform value is not an Int");

        return std::get<int>(_value);
    }

    void ShaderUniformValue::SetValue(const Vector2i& value)
    {
        _type = ShaderUniformType::Int2;
        _value = value;
    }

    Vector2i ShaderUniformValue::AsInt2() const
    {
        if (_type != ShaderUniformType::Int2)
            throw Exception("Uniform value is not an Int2");

        return std::get<Vector2i>(_value);
    }

    void ShaderUniformValue::SetValue(const Vector3i& value)
    {
        _type = ShaderUniformType::Int3;
        _value = value;
    }

    Vector3i ShaderUniformValue::AsInt3() const
    {
        if (_type != ShaderUniformType::Int3)
            throw Exception("Uniform value is not an Int3");

        return std::get<Vector3i>(_value);
    }

    void ShaderUniformValue::SetValue(const Vector4i& value)
    {
        _type = ShaderUniformType::Int4;
        _value = value;
    }

    Vector4i ShaderUniformValue::AsInt4() const
    {
        if (_type != ShaderUniformType::Int4)
            throw Exception("Uniform value is not an Int4");

        return std::get<Vector4i>(_value);
    }

    void ShaderUniformValue::SetValue(uint32 value)
    {
        _type = ShaderUniformType::UInt;
        _value = value;
    }

    uint32 ShaderUniformValue::AsUInt() const
    {
        if (_type != ShaderUniformType::UInt)
            throw Exception("Uniform value is not a UInt");

        return std::get<uint32>(_value);
    }

    void ShaderUniformValue::SetValue(Span<const uint32, 2> value)
    {
        _type = ShaderUniformType::UInt2;
        _value = Vector2i(static_cast<int>(value[0]), static_cast<int>(value[1]));
    }

    Span<const uint32, 2> ShaderUniformValue::AsUInt2() const
    {
        if (_type != ShaderUniformType::UInt2)
            throw Exception("Uniform value is not a UInt2");

        const Vector2i* v = &std::get<Vector2i>(_value);
        return Span<const uint32, 2>(reinterpret_cast<const uint32*>(v->Raw), 2);
    }

    void ShaderUniformValue::SetValue(Span<const uint32, 3> value)
    {
        _type = ShaderUniformType::UInt3;
        _value = Vector3i(static_cast<int>(value[0]), static_cast<int>(value[1]), static_cast<int>(value[2]));
    }

    Span<const uint32, 3> ShaderUniformValue::AsUInt3() const
    {
        if (_type != ShaderUniformType::UInt3)
            throw Exception("Uniform value is not a UInt3");

        const Vector3i* v = &std::get<Vector3i>(_value);
        return Span<const uint32, 3>(reinterpret_cast<const uint32*>(v->Raw), 3);
    }

    void ShaderUniformValue::SetValue(Span<const uint32, 4> value)
    {
        _type = ShaderUniformType::UInt4;
        _value = Vector4i(static_cast<int>(value[0]), static_cast<int>(value[1]), static_cast<int>(value[2]), static_cast<int>(value[3]));
    }

    Span<const uint32, 4> ShaderUniformValue::AsUInt4() const
    {
        if (_type != ShaderUniformType::UInt4)
            throw Exception("Uniform value is not a UInt4");

        const Vector4i* v = &std::get<Vector4i>(_value);
        return Span<const uint32, 4>(reinterpret_cast<const uint32*>(v->Raw), 4);
    }

    void ShaderUniformValue::SetValue(bool value)
    {
        _type = ShaderUniformType::Bool;
        _value = value;
    }

    bool ShaderUniformValue::AsBool() const
    {
        if (_type != ShaderUniformType::Bool)
            throw Exception("Uniform value is not a Bool");

        return std::get<bool>(_value);
    }

    void ShaderUniformValue::SetValue(const Color& color)
    {
        SetValue(color.AsVector4(false));
    }

    Color ShaderUniformValue::AsColor() const
    {
        return Color(AsFloat4(), false);
    }

    void ShaderUniformValue::SetValue(const Matrix4x4& value)
    {
        _type = ShaderUniformType::Matrix4x4;
        _value = value;
    }

    Matrix4x4 ShaderUniformValue::AsMatrix4x4() const
    {
        if (_type != ShaderUniformType::Matrix4x4)
            throw Exception("Uniform value is not a Matrix4x4");

        return std::get<Matrix4x4>(_value);
    }

    void ShaderUniformValue::SetValue(const SharedPtr<Texture>& value)
    {
        _type = ShaderUniformType::Texture;
        _value = value;
    }

    SharedPtr<Texture> ShaderUniformValue::AsTexture() const
    {
        if (_type != ShaderUniformType::Texture)
            throw Exception("Uniform value is not a Texture");

        return std::get<SharedPtr<Texture>>(_value);
    }

    void ShaderUniformValue::WriteInto(const ShaderCursor& cursor) const
    {
        ShaderCursor v = cursor.Field(_name.CStr());
        if (!v)
            return;

        switch (_type)
        {
            case ShaderUniformType::Float:
                v.Write(AsFloat());
                break;
            case ShaderUniformType::Float2:
                v.Write(AsFloat2());
                break;
            case ShaderUniformType::Float3:
                v.Write(AsFloat3());
                break;
            case ShaderUniformType::Float4:
                v.Write(AsFloat4());
                break;
            case ShaderUniformType::Int:
                v.Write(AsInt());
                break;
            case ShaderUniformType::Int2:
                v.Write(AsInt2());
                break;
            case ShaderUniformType::Int3:
                v.Write(AsInt3());
                break;
            case ShaderUniformType::Int4:
                v.Write(AsInt4());
                break;
            case ShaderUniformType::UInt:
                v.Write(AsUInt());
                break;
            case ShaderUniformType::UInt2:
                v.Write(AsUInt2());
                break;
            case ShaderUniformType::UInt3:
                v.Write(AsUInt3());
                break;
            case ShaderUniformType::UInt4:
                v.Write(AsUInt4());
                break;
            case ShaderUniformType::Bool:
                v.Write(AsBool());
                break;
            case ShaderUniformType::Matrix4x4:
                v.Write(AsMatrix4x4());
                break;
            case ShaderUniformType::Texture:
                v.Write(AsTexture());
                break;
        }
    }
} // Coco