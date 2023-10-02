#include "Renderpch.h"
#include "ImageSamplerDescriptionSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::Rendering
{
	void ImageSamplerDescriptionSerializer::Serialize(YAML::Emitter& emitter, const ImageSamplerDescription& description)
	{
		emitter << YAML::Key << "minimize filter" << YAML::Value << static_cast<int>(description.MinimizeFilter);
		emitter << YAML::Key << "magnify filter" << YAML::Value << static_cast<int>(description.MagnifyFilter);
		emitter << YAML::Key << "repeat U" << YAML::Value << static_cast<int>(description.RepeatModeU);
		emitter << YAML::Key << "repeat V" << YAML::Value << static_cast<int>(description.RepeatModeV);
		emitter << YAML::Key << "repeat W" << YAML::Value << static_cast<int>(description.RepeatModeW);
		emitter << YAML::Key << "mipmap filter" << YAML::Value << static_cast<int>(description.MipMapFilter);
		emitter << YAML::Key << "max anisotropy" << YAML::Value << static_cast<int>(description.MaxAnisotropy);
		emitter << YAML::Key << "min lod" << YAML::Value << description.MinLOD;
		emitter << YAML::Key << "max lod" << YAML::Value << description.MaxLOD;
		emitter << YAML::Key << "lod bias" << YAML::Value << description.LODBias;
	}

	ImageSamplerDescription ImageSamplerDescriptionSerializer::Deserialize(const YAML::Node& baseNode)
	{
		ImageFilterMode minimizeFilter = static_cast<ImageFilterMode>(baseNode["minimize filter"].as<int>());
		ImageFilterMode magnifyFilter = static_cast<ImageFilterMode>(baseNode["magnify filter"].as<int>());
		ImageRepeatMode repeatU = static_cast<ImageRepeatMode>(baseNode["repeat U"].as<int>());
		ImageRepeatMode repeatV = static_cast<ImageRepeatMode>(baseNode["repeat V"].as<int>());
		ImageRepeatMode repeatW = static_cast<ImageRepeatMode>(baseNode["repeat W"].as<int>());
		MipMapFilterMode mipmapFilter = static_cast<MipMapFilterMode>(baseNode["mipmap filter"].as<int>());
		uint8 maxAnisotropy = static_cast<uint8>(baseNode["max anisotropy"].as<int>());
		uint32 minLOD = baseNode["min lod"].as<uint32>();
		uint32 maxLOD = baseNode["max lod"].as<uint32>();
		double lodBias = baseNode["lod bias"].as<double>();

		return ImageSamplerDescription(minimizeFilter, magnifyFilter, repeatU, repeatV, repeatW, mipmapFilter, maxAnisotropy, minLOD, maxLOD, lodBias);
	}
}