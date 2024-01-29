#include "Renderpch.h"
#include "Material.h"


namespace Coco::Rendering
{
	Material::Material(const ResourceID& id) :
		Resource(id),
		_parameters()
	{}

	bool Material::HasValue(const string& name) const
	{
		return FindValue(name) != _parameters.end();
	}

	ShaderUniformType Material::GetValueType(const string& name) const
	{
		return FindValue(name)->Type;
	}

	void Material::MarkDirty()
	{
		IncrementVersion();
	}

	std::vector<ShaderUniformValue>::iterator Material::FindValue(const string& name)
	{
		return std::find_if(_parameters.begin(), _parameters.end(),
			[name](const ShaderUniformValue& value)
			{
				return value.Name == name;
			});
	}

	std::vector<ShaderUniformValue>::const_iterator Material::FindValue(const string& name) const
	{
		return std::find_if(_parameters.cbegin(), _parameters.cend(),
			[name](const ShaderUniformValue& value)
			{
				return value.Name == name;
			});
	}
}