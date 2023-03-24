#pragma once

namespace Coco
{
	/// @brief Types of built-in resources
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

	/// @brief Converts a resource type to its string representation
	/// @param type The resource type
	/// @return The string representation of the resource type
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