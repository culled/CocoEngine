#pragma once

namespace Coco::Rendering
{
	/// <summary>
	/// Types of shader stages
	/// </summary>
	enum class ShaderStageType
	{
		Vertex,
		Tesselation,
		Geometry,
		Fragment,
		Compute
	};

	enum class ShaderDescriptorType
	{
		UniformStruct,
		UniformSampler
	};
}