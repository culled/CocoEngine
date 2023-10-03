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

	void Shader::AddVariant(ShaderVariant&& passShader)
	{
		_variants.emplace_back(std::forward<ShaderVariant>(passShader));
	}

	bool Shader::TryGetShaderVariant(const char* variantName, const ShaderVariant*& outShaderVariant) const
	{
		for (const ShaderVariant& variant : _variants)
		{
			if (variant.Name == variantName)
			{
				outShaderVariant = &variant;
				return true;
			}
		}

		return false;
	}
}