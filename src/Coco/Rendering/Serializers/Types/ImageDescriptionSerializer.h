#pragma once

#include "../../Graphics/ImageTypes.h"

namespace YAML
{
	class Emitter;
	class Node;
}

namespace Coco::Rendering
{
	/// @brief Provides serialization for ImageDescriptions
	struct ImageDescriptionSerializer
	{
		/// @brief Serializes an ImageDescription
		/// @param emitter The emitter
		/// @param description The image description
		static void Serialize(YAML::Emitter& emitter, const ImageDescription& description);

		/// @brief Deserializes an ImageDescription
		/// @param baseNode The base node
		/// @return The image description
		static ImageDescription Deserialize(const YAML::Node& baseNode);
	};
}