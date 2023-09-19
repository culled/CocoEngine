#include "Renderpch.h"
#include "Shader.h"

namespace Coco::Rendering
{
	Shader::Shader(const string& groupTag) :
		_groupTag(groupTag),
		_passShaders{}
	{}

	Shader::~Shader()
	{
		_passShaders.clear();
	}

	uint64 Shader::GetID() const
	{
		// HACK: temporary
		std::hash<const Shader*> hasher;
		return hasher(this);
	}

	void Shader::SetGroupTag(const char* groupTag)
	{
		_groupTag = groupTag;
	}

	void Shader::AddRenderPassShader(RenderPassShader&& passShader)
	{
		RenderPassShader& shader = _passShaders.emplace_back(std::forward<RenderPassShader>(passShader));

		shader.ID = Math::CombineHashes(GetID(), _passShaders.size());
	}

	bool Shader::TryGetRenderPassShader(const char* renderPassName, const RenderPassShader*& outRenderPassShader) const
	{
		for (const RenderPassShader& passShader : _passShaders)
		{
			if (passShader.PassName == renderPassName)
			{
				outRenderPassShader = &passShader;
				return true;
			}
		}

		return false;
	}
}