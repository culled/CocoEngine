//
// Created by cullen on 4/4/26.
//

#include "Material.h"

#include "Coco/Core/Engine.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(Material, Resource);

    Material::Material(Engine* engine, uint64 id, SharedPtr<Shader> shader) :
        Resource(engine, id),
        _shader(shader),
        _uniformValues()
    {
        CreateUniformValues();
    }

    Material::~Material()
    {
        _uniformValues.Clear(true);
    }

    void Material::SetValue(const char* name, float value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Vector2& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Vector3& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Vector4& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, int value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Vector2i& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Vector3i& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Vector4i& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, uint32 value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, Span<const uint32, 2> value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, Span<const uint32, 3> value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, Span<const uint32, 4> value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, bool value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Color& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, const Matrix4x4& value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::SetValue(const char* name, SharedPtr<Texture> value)
    {
        if (auto uniform = TryGetUniformValue(name))
            uniform->SetValue(value);
    }

    void Material::CreateUniformValues()
    {
        if (!_shader)
            return;

        auto program = _shader->GetProgram();
        if (!program)
            return;

        auto programLayout = program->GetProgramLayout();
        auto globalTypeLayout = programLayout->getGlobalParamsTypeLayout();
        int64 materialBlockIndex = globalTypeLayout->findFieldIndexByName(MaterialBlockName);
        if (materialBlockIndex == -1)
        {
            COCO_ENGINE_LOG_ERROR("Failed to find material block named \"%s\" on shader \"%u\"", MaterialBlockName, _shader->GetID());
            return;
        }

        auto materialLayout = globalTypeLayout->getFieldByIndex(materialBlockIndex)->getTypeLayout()->getElementTypeLayout();
        auto fieldCount = materialLayout->getFieldCount();
        for (uint64 i = 0; i < fieldCount; i++)
        {
            auto field = materialLayout->getFieldByIndex(i);
            ShaderUniformValue uniform;
            if (!ShaderUniformValue::TryCreateFromField(field, uniform))
                continue;

            _uniformValues.Append(uniform);
        }
    }

    ShaderUniformValue* Material::TryGetUniformValue(const char* name)
    {
        for (auto& uniform : _uniformValues)
        {
            if (uniform.GetName() == name)
                return &uniform;
        }

        return nullptr;
    }
} // Coco