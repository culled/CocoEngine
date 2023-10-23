#pragma once

#include "../../Graphics/ImageSamplerTypes.h"

namespace YAML
{
	class Emitter;
	class Node;
}

namespace Coco::Rendering
{
	/// @brief Provides serialization for ImageSamplerDescriptions
	struct ImageSamplerDescriptionSerializer
	{
		/// @brief Serializes an ImageSamplerDescription
		/// @param emitter The emitter
		/// @param description The sampler description
		static void Serialize(YAML::Emitter& emitter, const ImageSamplerDescription& description);

		/// @brief Deserializes an ImageSamplerDescription
		/// @param baseNode The base node
		/// @return The sampler description
		static ImageSamplerDescription Deserialize(const YAML::Node& baseNode);
	};
}