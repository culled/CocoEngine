#include "Material.h"

#include "RenderingUtilities.h"
#include "RenderingService.h"
#include "Graphics/GraphicsDevice.h"

namespace Coco::Rendering
{
	Material::Material(const ResourceID& id, const string& name) : RenderingResource(id, name)
	{}

	Material::Material(const ResourceID& id, const string& name, Ref<Shader> shader) : RenderingResource(id, name),
		_shader(shader)
	{
		UpdatePropertyMaps(true);
	}

	Material::~Material()
	{}

	ShaderUniformData Material::GetUniformData() const
	{
		ShaderUniformData data(_uniformData);
		data.ID = ID;
		data.Version = GetMaterialVersion();

		return data;
	}

	void Material::SetShader(Ref<Shader> shader)
	{
		if (shader == _shader)
			return;

		_shader = shader;
		UpdatePropertyMaps(true);
		IncrementVersion();
	}

	void Material::SetInt(const string& name, int32_t value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Ints.find(name);

		if (it != _uniformData.Ints.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no int property named \"{}\"", _shader->GetName(), name));
		}
	}

	int32_t Material::GetInt(const string& name) const
	{
		const auto it = _uniformData.Ints.find(name);

		if (it != _uniformData.Ints.end())
		{
			return (*it).second;
		}
		else
		{
			return 0;
		}
	}

	void Material::SetVector2Int(const string& name, const Vector2Int& value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Vector2Ints.find(name);

		if (it != _uniformData.Vector2Ints.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector2int property named \"{}\"", _shader->GetName(), name));
		}
	}

	Vector2Int Material::GetVector2Int(const string& name) const
	{
		const auto it = _uniformData.Vector2Ints.find(name);

		if (it != _uniformData.Vector2Ints.end())
		{
			return (*it).second;
		}
		else
		{
			return Vector2Int::Zero;
		}
	}

	void Material::SetVector3Int(const string& name, const Vector3Int& value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Vector3Ints.find(name);

		if (it != _uniformData.Vector3Ints.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector3int property named \"{}\"", _shader->GetName(), name));
		}
	}

	Vector3Int Material::GetVector3Int(const string& name) const
	{
		const auto it = _uniformData.Vector3Ints.find(name);

		if (it != _uniformData.Vector3Ints.end())
		{
			return (*it).second;
		}
		else
		{
			return Vector3Int::Zero;
		}
	}

	void Material::SetVector4Int(const string& name, const Vector4Int& value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Vector4Ints.find(name);

		if (it != _uniformData.Vector4Ints.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector4int property named \"{}\"", _shader->GetName(), name));
		}
	}

	Vector4Int Material::GetVector4Int(const string& name) const
	{
		const auto it = _uniformData.Vector4Ints.find(name);

		if (it != _uniformData.Vector4Ints.end())
		{
			return (*it).second;
		}
		else
		{
			return Vector4Int::Zero;
		}
	}

	void Material::SetFloat(const string& name, float value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Floats.find(name);

		if (it != _uniformData.Floats.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no float property named \"{}\"", _shader->GetName(), name));
		}
	}

	float Material::GetFloat(const string& name) const
	{
		const auto it = _uniformData.Floats.find(name);

		if (it != _uniformData.Floats.end())
		{
			return (*it).second;
		}
		else
		{
			return 0.0f;
		}
	}

	void Material::SetVector2(const string& name, const Vector2& value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Vector2s.find(name);

		if (it != _uniformData.Vector2s.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector2 property named \"{}\"", _shader->GetName(), name));
		}
	}

	Vector2 Material::GetVector2(const string& name) const
	{
		const auto it = _uniformData.Vector2s.find(name);

		if (it != _uniformData.Vector2s.end())
		{
			return (*it).second;
		}
		else
		{
			return Vector2::Zero;
		}
	}

	void Material::SetVector3(const string& name, const Vector3& value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Vector3s.find(name);

		if (it != _uniformData.Vector3s.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector3 property named \"{}\"", _shader->GetName(), name));
		}
	}

	Vector3 Material::GetVector3(const string& name) const
	{
		const auto it = _uniformData.Vector3s.find(name);

		if (it != _uniformData.Vector3s.end())
		{
			return (*it).second;
		}
		else
		{
			return Vector3::Zero;
		}
	}

	void Material::SetVector4(const string& name, const Vector4& value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Vector4s.find(name);

		if (it != _uniformData.Vector4s.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no vector4 property named \"{}\"", _shader->GetName(), name));
		}
	}

	Vector4 Material::GetVector4(const string& name) const
	{
		const auto it = _uniformData.Vector4s.find(name);

		if (it != _uniformData.Vector4s.end())
		{
			return (*it).second;
		}
		else
		{
			return Vector4::Zero;
		}
	}

	void Material::SetColor(const string& name, const Color& value)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Colors.find(name);

		if (it != _uniformData.Colors.end())
		{
			(*it).second = value;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no color property named \"{}\"", _shader->GetName(), name));
		}
	}

	Color Material::GetColor(const string name) const
	{
		const auto it = _uniformData.Colors.find(name);

		if (it != _uniformData.Colors.end())
		{
			return (*it).second;
		}
		else
		{
			return Color::Black;
		}
	}

	void Material::SetTexture(const string& name, Ref<Texture> texture)
	{
		UpdatePropertyMaps(false);

		auto it = _uniformData.Textures.find(name);

		if (it != _uniformData.Textures.end())
		{
			(*it).second = texture->ID;
			this->IncrementVersion();
		}
		else
		{
			LogError(GetRenderingLogger(), FormattedString("Shader \"{}\" has no texture property named \"{}\"", _shader->GetName(), name));
		}
	}

	ResourceID Material::GetTexture(const string& name) const
	{
		const auto it = _uniformData.Textures.find(name);

		if (it != _uniformData.Textures.cend())
		{
			return (*it).second;
		}
		else
		{
			return Resource::InvalidID;
		}
	}

	void Material::UpdatePropertyMaps(bool forceUpdate)
	{
		if (!forceUpdate && _propertyMapVersion == _shader->GetVersion())
			return;

		ShaderUniformData shaderData = _shader->GetUniformPropertyMap();
		shaderData.CopyFrom(_uniformData);
		_uniformData = std::move(shaderData);
		
		_propertyMapVersion = _shader->GetVersion();
	}

	MaterialInstance::MaterialInstance(const ResourceID& id, const string& name, Ref<Material> baseMaterial) :
		RenderingResource(id, name),
		_baseMaterial(baseMaterial)
	{}

	ResourceVersion MaterialInstance::GetMaterialVersion() const
	{
		return GetVersion() + _baseMaterial->GetMaterialVersion();
	}

	ShaderUniformData MaterialInstance::GetUniformData() const
	{
		ShaderUniformData data(_baseMaterial->GetUniformData());
		data.ID = ID;
		data.Version = GetMaterialVersion();
		data.Merge(_uniformData, true);

		return data;
	}

	void MaterialInstance::SetInt(const string& name, int32_t value)
	{
		_uniformData.Ints[name] = value;
		IncrementVersion();
	}

	int32_t MaterialInstance::GetInt(const string& name) const
	{
		if (_uniformData.Ints.contains(name))
			return _uniformData.Ints.at(name);

		return _baseMaterial->GetInt(name);
	}

	UnorderedMap<string, int32_t> MaterialInstance::GetIntProperties() const noexcept
	{
		UnorderedMap<string, int32_t> props(_baseMaterial->GetIntProperties());

		for (const auto& kvp : _uniformData.Ints)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetVector2Int(const string& name, const Vector2Int& value)
	{
		_uniformData.Vector2Ints[name] = value;
		IncrementVersion();
	}

	Vector2Int MaterialInstance::GetVector2Int(const string& name) const
	{
		if (_uniformData.Vector2Ints.contains(name))
			return _uniformData.Vector2Ints.at(name);

		return _baseMaterial->GetVector2Int(name);
	}

	UnorderedMap<string, Vector2Int> MaterialInstance::GetVector2IntProperties() const noexcept
	{
		UnorderedMap<string, Vector2Int> props(_baseMaterial->GetVector2IntProperties());

		for (const auto& kvp : _uniformData.Vector2Ints)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetVector3Int(const string& name, const Vector3Int& value)
	{
		_uniformData.Vector3Ints[name] = value;
		IncrementVersion();
	}

	Vector3Int MaterialInstance::GetVector3Int(const string& name) const
	{
		if (_uniformData.Vector3Ints.contains(name))
			return _uniformData.Vector3Ints.at(name);

		return _baseMaterial->GetVector3Int(name);
	}

	UnorderedMap<string, Vector3Int> MaterialInstance::GetVector3IntProperties() const noexcept
	{
		UnorderedMap<string, Vector3Int> props(_baseMaterial->GetVector3IntProperties());

		for (const auto& kvp : _uniformData.Vector3Ints)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetVector4Int(const string& name, const Vector4Int& value)
	{
		_uniformData.Vector4Ints[name] = value;
		IncrementVersion();
	}

	Vector4Int MaterialInstance::GetVector4Int(const string& name) const
	{
		if (_uniformData.Vector4Ints.contains(name))
			return _uniformData.Vector4Ints.at(name);

		return _baseMaterial->GetVector4Int(name);
	}

	UnorderedMap<string, Vector4Int> MaterialInstance::GetVector4IntProperties() const noexcept
	{
		UnorderedMap<string, Vector4Int> props(_baseMaterial->GetVector4IntProperties());

		for (const auto& kvp : _uniformData.Vector4Ints)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetFloat(const string& name, float value)
	{
		_uniformData.Floats[name] = value;
		IncrementVersion();
	}

	float MaterialInstance::GetFloat(const string& name) const
	{
		if (_uniformData.Floats.contains(name))
			return _uniformData.Floats.at(name);

		return _baseMaterial->GetFloat(name);
	}

	UnorderedMap<string, float> MaterialInstance::GetFloatProperties() const noexcept
	{
		UnorderedMap<string, float> props(_baseMaterial->GetFloatProperties());

		for (const auto& kvp : _uniformData.Floats)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetVector2(const string& name, const Vector2& value)
	{
		_uniformData.Vector2s[name] = value;
		IncrementVersion();
	}

	Vector2 MaterialInstance::GetVector2(const string& name) const
	{
		if (_uniformData.Vector2s.contains(name))
			return _uniformData.Vector2s.at(name);

		return _baseMaterial->GetVector2(name);
	}

	UnorderedMap<string, Vector2> MaterialInstance::GetVector2Properties() const noexcept
	{
		UnorderedMap<string, Vector2> props(_baseMaterial->GetVector2Properties());

		for (const auto& kvp : _uniformData.Vector2s)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetVector3(const string& name, const Vector3& value)
	{
		_uniformData.Vector3s[name] = value;
		IncrementVersion();
	}

	Vector3 MaterialInstance::GetVector3(const string& name) const
	{
		if (_uniformData.Vector3s.contains(name))
			return _uniformData.Vector3s.at(name);

		return _baseMaterial->GetVector3(name);
	}

	UnorderedMap<string, Vector3> MaterialInstance::GetVector3Properties() const noexcept
	{
		UnorderedMap<string, Vector3> props(_baseMaterial->GetVector3Properties());

		for (const auto& kvp : _uniformData.Vector3s)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetVector4(const string& name, const Vector4& value)
	{
		_uniformData.Vector4s[name] = value;
		IncrementVersion();
	}

	Vector4 MaterialInstance::GetVector4(const string& name) const
	{
		if (_uniformData.Vector4s.contains(name))
			return _uniformData.Vector4s.at(name);

		return _baseMaterial->GetVector4(name);
	}

	UnorderedMap<string, Vector4> MaterialInstance::GetVector4Properties() const noexcept
	{
		UnorderedMap<string, Vector4> props(_baseMaterial->GetVector4Properties());

		for (const auto& kvp : _uniformData.Vector4s)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetColor(const string& name, const Color& value)
	{
		_uniformData.Colors[name] = value;
		IncrementVersion();
	}

	Color MaterialInstance::GetColor(const string name) const
	{
		if (_uniformData.Colors.contains(name))
			return _uniformData.Colors.at(name);

		return _baseMaterial->GetColor(name);
	}

	UnorderedMap<string, Color> MaterialInstance::GetColorProperties() const noexcept
	{
		UnorderedMap<string, Color> props(_baseMaterial->GetColorProperties());

		for (const auto& kvp : _uniformData.Colors)
			props[kvp.first] = kvp.second;

		return props;
	}

	void MaterialInstance::SetTexture(const string& name, Ref<Texture> texture)
	{
		_uniformData.Textures[name] = texture->ID;
		IncrementVersion();
	}

	ResourceID MaterialInstance::GetTexture(const string& name) const
	{
		if (_uniformData.Textures.contains(name))
			return _uniformData.Textures.at(name);

		return _baseMaterial->GetTexture(name);
	}

	UnorderedMap<string, ResourceID> MaterialInstance::GetTextureProperties() const noexcept
	{
		UnorderedMap<string, ResourceID> props(_baseMaterial->GetTextureProperties());

		for (const auto& kvp : _uniformData.Textures)
			props[kvp.first] = kvp.second;

		return props;
	}
}
