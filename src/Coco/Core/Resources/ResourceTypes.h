#pragma once

namespace Coco
{
	/// <summary>
	/// Types of builtin resources
	/// </summary>
	enum class ResourceType
	{
		Text,
		Binary,
		Texture,
		Shader,
		Material,
		Mesh,
		RenderPipeline,
	};

	/// <summary>
	/// Converts a resource type to its string representation
	/// </summary>
	/// <param name="type">The resource type</param>
	/// <returns>The string representation of the resource type</returns>
	constexpr const char* ResourceTypeToString(ResourceType type)
	{
		switch (type)
		{
		case ResourceType::Text:
			return "Text";
		case ResourceType::Binary:
			return "Binary";
		case ResourceType::Texture:
			return "Texture";
		case ResourceType::Shader:
			return "Shader";
		case ResourceType::Material:
			return "Material";
		case ResourceType::Mesh:
			return "Mesh";
		case ResourceType::RenderPipeline:
				return "RenderPipeline";
		default:
			return "Unknown";
		}
	}
}