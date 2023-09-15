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

	void Shader::AddRenderPassShader(RenderPassShader&& passShader)
	{
		_passShaders.emplace_back(std::forward<RenderPassShader>(passShader));
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