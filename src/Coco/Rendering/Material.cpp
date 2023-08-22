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
