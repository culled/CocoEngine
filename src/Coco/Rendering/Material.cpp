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

	template<>
	ShaderUniformUnion MaterialParameter::As<ShaderUniformUnion>() const
	{
		Assert(IsDataShaderUniformType(Type))

		switch (Type)
		{
		case ShaderUniformType::Float:
			return As<float>();
		case ShaderUniformType::Float2:
			return As<Vector2>();
		case ShaderUniformType::Float3:
			return As<Vector3>();
		case ShaderUniformType::Float4:
			return As<Vector4>();
		case ShaderUniformType::Color:
			return As<Color>();
		case ShaderUniformType::Mat4x4:
			return As<Matrix4x4>();
		case ShaderUniformType::Int:
			return As<int>();
		case ShaderUniformType::Int2:
			return As<Vector2Int>();
		case ShaderUniformType::Int3:
			return As<Vector3Int>();
		case ShaderUniformType::Int4:
			return As<Vector4Int>();
		case ShaderUniformType::Bool:
			return As<bool>();
		default:
		{
			Assert(false)
			return ShaderUniformUnion();
		}
		}
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

				if (param.Type == ShaderUniformType::Texture)
				{
					SharedRef<Texture> tex = param.As<SharedRef<Texture>>();

					if (tex)
						data.Textures[key] = ShaderUniformData::ToTextureSampler(tex->GetImage(), tex->GetImageSampler());
				}
				else
				{
					data.Uniforms[key] = param.As<ShaderUniformUnion>();
				}
			}
		);

		return data;
	}

	bool Material::HasParameter(const char* name) const
	{
		return _parameters.contains(name);
	}

	void Material::AddParameter(const char* name, ShaderUniformType type)
	{
		if (_parameters.contains(name))
			return;

		std::any value;

		switch (type)
		{
		case ShaderUniformType::Float:
			value = 0.f;
			break;
		case ShaderUniformType::Float2:
			value = Vector2::Zero;
			break;
		case ShaderUniformType::Float3:
			value = Vector3::Zero;
			break; 
		case ShaderUniformType::Float4:
			value = Vector4::Zero;
			break;
		case ShaderUniformType::Color:
			value = Color::Black;
			break;
		case ShaderUniformType::Mat4x4:
			value = Matrix4x4::Identity;
			break;
		case ShaderUniformType::Int:
			value = 0;
			break;
		case ShaderUniformType::Int2:
			value = Vector2Int::Zero;
			break;
		case ShaderUniformType::Int3:
			value = Vector3Int::Zero;
			break;
		case ShaderUniformType::Int4:
			value = Vector4Int::Zero;
			break;
		case ShaderUniformType::Bool:
			value = false;
			break;
		case ShaderUniformType::Texture:
			value = SharedRef<Texture>();
			break;
		default:
			break;
		}

		_parameters.try_emplace(name, MaterialParameter(name, type, value));
	}

	void Material::RemoveParameter(const char* name)
	{
		auto it = _parameters.find(name);

		if (it == _parameters.end())
			return;

		_parameters.erase(it);
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