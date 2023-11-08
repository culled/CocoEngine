#include "Renderpch.h"
#include "Material.h"

#include "RenderService.h"

namespace Coco::Rendering
{
	MaterialParameter::MaterialParameter() :
		MaterialParameter("", ShaderUniformType::Float, std::any())
	{}

	MaterialParameter::MaterialParameter(const string& name, ShaderUniformType type, std::any value) :
		Name(name),
		Type(type),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, float value) :
		Name(name),
		Type(ShaderUniformType::Float),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string & name, const Vector2 & value) :
		Name(name),
		Type(ShaderUniformType::Float2),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, const Vector3& value) :
		Name(name),
		Type(ShaderUniformType::Float3),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, const Vector4& value) :
		Name(name),
		Type(ShaderUniformType::Float4),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, const Color& value) :
		Name(name),
		Type(ShaderUniformType::Color),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, const Matrix4x4& value) :
		Name(name),
		Type(ShaderUniformType::Mat4x4),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, int value) :
		Name(name),
		Type(ShaderUniformType::Int),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, const Vector2Int& value) :
		Name(name),
		Type(ShaderUniformType::Int2),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, const Vector3Int& value) :
		Name(name),
		Type(ShaderUniformType::Int3),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, const Vector4Int& value) :
		Name(name),
		Type(ShaderUniformType::Int4),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string& name, bool value) :
		Name(name),
		Type(ShaderUniformType::Bool),
		Value(value)
	{}

	MaterialParameter::MaterialParameter(const string & name, const SharedRef<Texture>&value) :
		Name(name),
		Type(ShaderUniformType::Texture),
		Value(value)
	{}

	template<>
	float MaterialParameter::As<float>() const
	{
		Assert(Type == ShaderUniformType::Float)
		return std::any_cast<float>(Value);
	}

	template<>
	Vector2 MaterialParameter::As<Vector2>() const
	{
		Assert(Type == ShaderUniformType::Float2)
		return std::any_cast<Vector2>(Value);
	}

	template<>
	Vector3 MaterialParameter::As<Vector3>() const
	{
		Assert(Type == ShaderUniformType::Float3)
		return std::any_cast<Vector3>(Value);
	}

	template<>
	Vector4 MaterialParameter::As<Vector4>() const
	{
		Assert(Type == ShaderUniformType::Float4)
		return std::any_cast<Vector4>(Value);
	}

	template<>
	Color MaterialParameter::As<Color>() const
	{
		Assert(Type == ShaderUniformType::Color)
		return std::any_cast<Color>(Value);
	}

	template<>
	Matrix4x4 MaterialParameter::As<Matrix4x4>() const
	{
		Assert(Type == ShaderUniformType::Mat4x4)
		return std::any_cast<Matrix4x4>(Value);
	}

	template<>
	int MaterialParameter::As<int>() const
	{
		Assert(Type == ShaderUniformType::Int)
		return std::any_cast<int>(Value);
	}

	template<>
	Vector2Int MaterialParameter::As<Vector2Int>() const
	{
		Assert(Type == ShaderUniformType::Int2)
		return std::any_cast<Vector2Int>(Value);
	}

	template<>
	Vector3Int MaterialParameter::As<Vector3Int>() const
	{
		Assert(Type == ShaderUniformType::Int3)
		return std::any_cast<Vector3Int>(Value);
	}

	template<>
	Vector4Int MaterialParameter::As<Vector4Int>() const
	{
		Assert(Type == ShaderUniformType::Int4)
		return std::any_cast<Vector4Int>(Value);
	}

	template<>
	bool MaterialParameter::As<bool>() const
	{
		Assert(Type == ShaderUniformType::Bool)
		return std::any_cast<bool>(Value);
	}

	template<>
	SharedRef<Texture> MaterialParameter::As<SharedRef<Texture>>() const
	{
		Assert(Type == ShaderUniformType::Texture)
		return std::any_cast<SharedRef<Texture>>(Value);
	}

	Material::Material(const ResourceID& id, const string& name) :
		RendererResource(id, name),
		_parameters()
	{}

	ShaderUniformData Material::GetUniformData() const
	{
		ShaderUniformData data(GetVersion());

		ForEachParameter(
			[&data](const MaterialParameter& param)
			{
				ShaderUniformData::UniformKey key = ShaderUniformData::MakeKey(param.Name);

				switch (param.Type)
				{
				case ShaderUniformType::Float:
					data.Uniforms[key] = param.As<float>();
					break;
				case ShaderUniformType::Float2:
					data.Uniforms[key] = param.As<Vector2>();
					break;
				case ShaderUniformType::Float3:
					data.Uniforms[key] = param.As<Vector3>();
					break;
				case ShaderUniformType::Float4:
					data.Uniforms[key] = param.As<Vector4>();
					break;
				case ShaderUniformType::Color:
					data.Uniforms[key] = param.As<Color>();
					break;
				case ShaderUniformType::Mat4x4:
					data.Uniforms[key] = param.As<Matrix4x4>();
					break;
				case ShaderUniformType::Int:
					data.Uniforms[key] = param.As<int>();
					break;
				case ShaderUniformType::Int2:
					data.Uniforms[key] = param.As<Vector2Int>();
					break;
				case ShaderUniformType::Int3:
					data.Uniforms[key] = param.As<Vector3Int>();
					break;
				case ShaderUniformType::Int4:
					data.Uniforms[key] = param.As<Vector4Int>();
					break;
				case ShaderUniformType::Bool:
					data.Uniforms[key] = param.As<bool>();
					break;
				case ShaderUniformType::Texture:
				{
					SharedRef<Texture> tex = param.As<SharedRef<Texture>>();

					if (tex)
						data.Textures[key] = ShaderUniformData::ToTextureSampler(tex->GetImage(), tex->GetImageSampler());

					break;
				}
				default:
					break;
				}
			}
		);

		return data;
	}

	bool Material::HasParameter(const char* name) const
	{
		return _parameters.contains(name);
	}

	void Material::ForEachParameter(std::function<void(const MaterialParameter&)> callback) const
	{
		for (const auto& kvp : _parameters)
		{
			callback(kvp.second);
		}
	}

	void Material::AddParametersFromShader(const Shader& shader)
	{
		const ShaderUniformLayout& layout = shader.GetInstanceUniformLayout();

		for (const auto& u : layout.Uniforms)
		{
			std::any value;
			if (u.Type == ShaderUniformType::Texture)
			{
				switch (std::any_cast<DefaultTextureType>(u.DefaultValue))
				{
				case DefaultTextureType::White:
					value = RenderService::Get()->GetDefaultDiffuseTexture();
					break;
				case DefaultTextureType::FlatNormal:
					value = RenderService::Get()->GetDefaultNormalTexture();
					break;
				default:
					value = RenderService::Get()->GetDefaultCheckerTexture();
					break;
				}
			}
			else
			{
				value = u.DefaultValue;
			}

			_parameters[u.Name] = MaterialParameter(u.Name, u.Type, value);
		}
	}

	void Material::IncrementVersion()
	{
		SetVersion(GetVersion() + 1);
	}
}