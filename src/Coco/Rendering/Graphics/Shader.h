#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include "ShaderTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// Defines how geometry is rendered
	/// </summary>
	class COCOAPI Shader
	{
	private:
		string _name;
		Map<ShaderStageType, string> _stageFiles;

	public:
		Shader(const string& name);
		virtual ~Shader() = default;

		/// <summary>
		/// Gets this shader's name
		/// </summary>
		/// <returns>This shader's name</returns>
		string GetName() const { return _name; }

		/// <summary>
		/// Gets the map of shader stages and their files
		/// </summary>
		/// <returns>The map of shader stages and their files</returns>
		Map<ShaderStageType, string> GetStageSources() const { return _stageFiles; } // TODO: support non-file shaders?
	};
}