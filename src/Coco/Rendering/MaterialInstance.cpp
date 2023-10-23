#include "Renderpch.h"
#include "MaterialInstance.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	MaterialInstance::MaterialInstance(const ResourceID& id, const string& name, SharedRef<Material> baseMaterial) :
		RendererResource(id, name),
		_baseMaterial(baseMaterial),
		_uniformData(0)
	{
	}

	MaterialInstance::~MaterialInstance()
	{
		_baseMaterial.reset();
	}

	ShaderUniformData MaterialInstance::GetUniformData() const
	{
		if (!_baseMaterial)
		{
			CocoError("Base material is invalid")
			return _uniformData;
		}

		ShaderUniformData uniforms = _baseMaterial->GetUniformData();
		uniforms.MergeWith(_uniformData);
		uniforms.Version += _uniformData.Version;

		return uniforms;
	}

	SharedRef<Shader> MaterialInstance::GetShader() const
	{
		if (!_baseMaterial)
		{
			CocoError("Base material is invalid")
			return nullptr;
		}

		return _baseMaterial->GetShader();
	}

	void MaterialInstance::SetFloat(const char* name, float value)
	{
		_uniformData.Floats[ShaderUniformData::MakeKey(name)] = value;

		IncrementVersion();
	}

	float MaterialInstance::GetFloat(const char* name) const
	{
		auto it = _uniformData.Floats.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Floats.end())
			return 0.0f;

		return it->second;
	}

	void MaterialInstance::SetFloat2(const char* name, const Vector2& value)
	{
		_uniformData.Float2s[ShaderUniformData::MakeKey(name)] = ShaderUniformData::ToFloat2(value);

		IncrementVersion();
	}

	Vector2 MaterialInstance::GetFloat2(const char* name) const
	{
		auto it = _uniformData.Float2s.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Float2s.end())
			return Vector2::Zero;

		const ShaderUniformData::float2& v = it->second;
		return Vector2(v[0], v[1]);
	}

	void MaterialInstance::SetFloat3(const char* name, const Vector3& value)
	{
		_uniformData.Float3s[ShaderUniformData::MakeKey(name)] = ShaderUniformData::ToFloat3(value);

		IncrementVersion();
	}

	Vector3 MaterialInstance::GetFloat3(const char* name) const
	{
		auto it = _uniformData.Float3s.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Float3s.end())
			return Vector3::Zero;

		const ShaderUniformData::float3& v = it->second;
		return Vector3(v[0], v[1], v[2]);
	}

	void MaterialInstance::SetFloat4(const char* name, const Vector4& value)
	{
		_uniformData.Float4s[ShaderUniformData::MakeKey(name)] = ShaderUniformData::ToFloat4(value);

		IncrementVersion();
	}

	void MaterialInstance::SetFloat4(const char* name, const Color& value, bool asLinear)
	{
		Color c = asLinear ? value.AsLinear() : value.AsGamma();
		SetFloat4(name, Vector4(c.R, c.G, c.B, c.A));
	}

	Vector4 MaterialInstance::GetFloat4(const char* name) const
	{
		auto it = _uniformData.Float4s.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Float4s.end())
			return Vector4::Zero;

		const ShaderUniformData::float4& v = it->second;
		return Vector4(v[0], v[1], v[2], v[3]);
	}

	void MaterialInstance::SetMatrix4x4(const char* name, const Matrix4x4& value)
	{
		_uniformData.Mat4x4s[ShaderUniformData::MakeKey(name)] = ShaderUniformData::ToMat4x4(value);

		IncrementVersion();
	}

	Matrix4x4 MaterialInstance::GetMatrix4x4(const char* name) const
	{
		auto it = _uniformData.Mat4x4s.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Mat4x4s.end())
			return Matrix4x4();

		const ShaderUniformData::Mat4x4& v = it->second;
		Matrix4x4 mat;

		for (uint8 i = 0; i < Matrix4x4::CellCount; i++)
		{
			mat.Data[i] = v[i];
		}

		return mat;
	}

	void MaterialInstance::SetInt(const char* name, int32 value)
	{
		_uniformData.Ints[ShaderUniformData::MakeKey(name)] = value;

		IncrementVersion();
	}

	int32 MaterialInstance::GetInt(const char* name) const
	{
		auto it = _uniformData.Ints.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Ints.end())
			return 0;

		return it->second;
	}

	void MaterialInstance::SetInt2(const char* name, const Vector2Int& value)
	{
		_uniformData.Int2s[ShaderUniformData::MakeKey(name)] = ShaderUniformData::ToInt2(value);

		IncrementVersion();
	}

	Vector2Int MaterialInstance::GetInt2(const char* name) const
	{
		auto it = _uniformData.Int2s.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Int2s.end())
			return Vector2Int::Zero;

		const ShaderUniformData::int2& v = it->second;
		return Vector2Int(v[0], v[1]);
	}

	void MaterialInstance::SetInt3(const char* name, const Vector3Int& value)
	{
		_uniformData.Int3s[ShaderUniformData::MakeKey(name)] = ShaderUniformData::ToInt3(value);

		IncrementVersion();
	}

	Vector3Int MaterialInstance::GetInt3(const char* name) const
	{
		auto it = _uniformData.Int3s.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Int3s.end())
			return Vector3Int::Zero;

		const ShaderUniformData::int3& v = it->second;
		return Vector3Int(v[0], v[1], v[2]);
	}

	void MaterialInstance::SetInt4(const char* name, const Vector4Int& value)
	{
		_uniformData.Int4s[ShaderUniformData::MakeKey(name)] = ShaderUniformData::ToInt4(value);

		IncrementVersion();
	}

	Vector4Int MaterialInstance::GetInt4(const char* name) const
	{
		auto it = _uniformData.Int4s.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Int4s.end())
			return Vector4Int::Zero;

		const ShaderUniformData::int4& v = it->second;
		return Vector4Int(v[0], v[1], v[2], v[3]);
	}

	void MaterialInstance::SetBool(const char* name, bool value)
	{
		_uniformData.Bools[ShaderUniformData::MakeKey(name)] = value;

		IncrementVersion();
	}

	bool MaterialInstance::GetBool(const char* name) const
	{
		auto it = _uniformData.Bools.find(ShaderUniformData::MakeKey(name));

		if (it == _uniformData.Bools.end())
			return false;

		return it->second == 1;
	}

	void MaterialInstance::SetTexture(const char* name, SharedRef<Texture> texture)
	{
		ShaderUniformData::UniformKey key = ShaderUniformData::MakeKey(name);

		if (texture)
		{
			_textures[key] = texture;
			_uniformData.Textures[key] = ShaderUniformData::ToTextureSampler(texture->GetImage(), texture->GetImageSampler());
		}
		else
		{
			auto textureIt = _textures.find(key);
			if (textureIt != _textures.end())
				_textures.erase(textureIt);

			auto uniformIt = _uniformData.Textures.find(key);
			if (uniformIt != _uniformData.Textures.end())
				_uniformData.Textures.erase(uniformIt);
		}

		IncrementVersion();
	}

	SharedRef<Texture> MaterialInstance::GetTexture(const char* name) const
	{
		auto it = _textures.find(ShaderUniformData::MakeKey(name));

		if (it == _textures.end())
			return nullptr;

		return it->second;
	}

	void MaterialInstance::IncrementVersion()
	{
		_uniformData.Version++;
		SetVersion(_uniformData.Version);
	}
}