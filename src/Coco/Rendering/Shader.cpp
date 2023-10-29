#include "Renderpch.h"
#include "Shader.h"

namespace Coco::Rendering
{
	Shader::Shader(const ResourceID& id, const string& name, const string& groupTag) :
		RendererResource(id, name),
		_groupTag(groupTag),
		_variants{}
	{}

	Shader::~Shader()
	{
		_variants.clear();
	}

	void Shader::SetGroupTag(const char* groupTag)
	{
		_groupTag = groupTag;
	}

	void Shader::AddVariant(ShaderVariant&& variant)
	{
		_variants.emplace_back(std::forward<ShaderVariant>(variant));
	}

	void Shader::AddVariant(const ShaderVariant& variant)
	{
		_variants.push_back(variant);
	}

	bool Shader::TryGetShaderVariant(const char* variantName, const ShaderVariant*& outShaderVariant) const
	{
		auto it = std::find_if(_variants.begin(), _variants.end(), [variantName](const ShaderVariant& variant) { return variant.Name == variantName; });

		if (it == _variants.end())
			return false;

		outShaderVariant = &(*it);

		return true;
	}
}