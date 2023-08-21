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

	//MaterialInstance::MaterialInstance(const Material* material) : Material(material->Shader, FormattedString("{} (Instance)", material->Name))
	//{}
	//
	//MaterialInstance::~MaterialInstance()
	//{}
}
