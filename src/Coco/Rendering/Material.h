//
// Created by cullen on 4/4/26.
//

#ifndef COCOENGINE_MATERIAL_H
#define COCOENGINE_MATERIAL_H
#include "Shader.h"

#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Resources/Resource.h"
#include "Coco/Core/Types/Array.h"

#include "Graphics/ShaderUniformValue.h"

namespace Coco
{
    class Material : public Resource
    {
        DECLARE_RTTI_TYPE(Material)

    public:
        static constexpr const char* MaterialBlockName = "materialData";

    public:
        Material(Engine* engine, uint64 id, SharedPtr<Shader> shader);
        ~Material();

        void SetValue(const char* name, float value);
        void SetValue(const char* name, const Vector2& value);
        void SetValue(const char* name, const Vector3& value);
        void SetValue(const char* name, const Vector4& value);
        void SetValue(const char* name, int value);
        void SetValue(const char* name, const Vector2i& value);
        void SetValue(const char* name, const Vector3i& value);
        void SetValue(const char* name, const Vector4i& value);
        void SetValue(const char* name, uint32 value);
        void SetValue(const char* name, Span<const uint32, 2> value);
        void SetValue(const char* name, Span<const uint32, 3> value);
        void SetValue(const char* name, Span<const uint32, 4> value);
        void SetValue(const char* name, bool value);
        void SetValue(const char* name, const Color& value);
        void SetValue(const char* name, const Matrix4x4& value);
        void SetValue(const char* name, SharedPtr<Texture> value);

        Span<const ShaderUniformValue> GetUniformValues() const { return _uniformValues; }
        SharedPtr<Shader> GetShader() const { return _shader; }

    private:
        Array<ShaderUniformValue> _uniformValues;
        SharedPtr<Shader> _shader;

    private:
        void CreateUniformValues();
        ShaderUniformValue* TryGetUniformValue(const char* name);
    };
} // Coco

#endif //COCOENGINE_MATERIAL_H