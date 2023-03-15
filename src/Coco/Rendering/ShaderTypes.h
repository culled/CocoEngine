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

	/// <summary>
	/// Types of shader descriptors
	/// </summary>
	enum class ShaderDescriptorType
	{
		UniformStruct,
		UniformSampler
	};
}