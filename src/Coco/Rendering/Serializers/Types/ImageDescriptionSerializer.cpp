#include "Renderpch.h"
#include "ImageDescriptionSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::Rendering
{
	void ImageDescriptionSerializer::Serialize(YAML::Emitter& emitter, const ImageDescription& description)
	{
		emitter << YAML::Key << "width" << YAML::Value << description.Width;
		emitter << YAML::Key << "height" << YAML::Value << description.Height;
		emitter << YAML::Key << "depth" << YAML::Value << description.Depth;
		emitter << YAML::Key << "layers" << YAML::Value << description.Layers;
		emitter << YAML::Key << "pixel format" << YAML::Value << static_cast<int>(description.PixelFormat);
		emitter << YAML::Key << "color space" << YAML::Value << static_cast<int>(description.ColorSpace);
		emitter << YAML::Key << "mipmaps" << YAML::Value << (description.MipCount > 1);
		emitter << YAML::Key << "usage flags" << YAML::Value << static_cast<int>(description.UsageFlags);
		emitter << YAML::Key << "sample count" << YAML::Value << static_cast<int>(description.SampleCount);
	}

	ImageDescription ImageDescriptionSerializer::Deserialize(const YAML::Node& baseNode)
	{
		uint32 width = baseNode["width"].as<uint32>();
		uint32 height = baseNode["height"].as<uint32>();
		uint32 depth = baseNode["depth"].as<uint32>();
		uint32 layers = baseNode["layers"].as<uint32>();
		ImagePixelFormat pixelFormat = static_cast<ImagePixelFormat>(baseNode["pixel format"].as<int>());
		ImageColorSpace colorSpace = static_cast<ImageColorSpace>(baseNode["color space"].as<int>());
		bool mipmaps = baseNode["mipmaps"].as<bool>();
		ImageUsageFlags usageFlags = static_cast<ImageUsageFlags>(baseNode["usage flags"].as<int>());
		MSAASamples samples = static_cast<MSAASamples>(baseNode["sample count"].as<int>());

		return ImageDescription(width, height, depth, layers, pixelFormat, colorSpace, usageFlags, mipmaps, samples);
	}
}