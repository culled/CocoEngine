#include "Renderpch.h"
#include "TextureSerializer.h"
#include "../Texture.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	bool TextureSerializer::SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource)
	{
		SharedRef<Texture> texture = std::dynamic_pointer_cast<Texture>(resource);

		if (!texture)
		{
			CocoError("Resource was not a texture resource")
			return false;
		}

		if (!texture->_sampler.IsValid())
		{
			CocoError("Texture's sampler was not valid")
			return false;
		}

		emitter << YAML::Key << "imageFilePath" << YAML::Value << texture->_imageFilePath.ToString();

		emitter << YAML::Key << "imageDescription" << YAML::Value << YAML::BeginMap;

		const ImageDescription& description = texture->_imageDescription;
		emitter << YAML::Key << "width" << YAML::Value << description.Width;
		emitter << YAML::Key << "height" << YAML::Value << description.Height;
		emitter << YAML::Key << "depth" << YAML::Value << description.Depth;
		emitter << YAML::Key << "layers" << YAML::Value << description.Layers;
		emitter << YAML::Key << "pixelFormat" << YAML::Value << static_cast<int>(description.PixelFormat);
		emitter << YAML::Key << "colorSpace" << YAML::Value << static_cast<int>(description.ColorSpace);
		emitter << YAML::Key << "mipmapCount" << YAML::Value << description.MipCount;
		emitter << YAML::Key << "usageFlags" << YAML::Value << static_cast<int>(description.UsageFlags);
		emitter << YAML::Key << "sampleCount" << YAML::Value << static_cast<int>(description.SampleCount);

		emitter << YAML::EndMap;

		emitter << YAML::Key << "samplerDescription" << YAML::Value << YAML::BeginMap;

		const ImageSamplerDescription& samplerDescription = texture->_sampler->GetDescription();
		emitter << YAML::Key << "minimizeFilter" << YAML::Value << static_cast<int>(samplerDescription.MinimizeFilter);
		emitter << YAML::Key << "magnifyFilter" << YAML::Value << static_cast<int>(samplerDescription.MagnifyFilter);
		emitter << YAML::Key << "repeatU" << YAML::Value << static_cast<int>(samplerDescription.RepeatModeU);
		emitter << YAML::Key << "repeatV" << YAML::Value << static_cast<int>(samplerDescription.RepeatModeV);
		emitter << YAML::Key << "repeatW" << YAML::Value << static_cast<int>(samplerDescription.RepeatModeW);
		emitter << YAML::Key << "mipmapFilter" << YAML::Value << static_cast<int>(samplerDescription.MipMapFilter);
		emitter << YAML::Key << "maxAnisotropy" << YAML::Value << samplerDescription.MaxAnisotropy;
		emitter << YAML::Key << "minLod" << YAML::Value << samplerDescription.MinLOD;
		emitter << YAML::Key << "maxLod" << YAML::Value << samplerDescription.MaxLOD;
		emitter << YAML::Key << "lodBias" << YAML::Value << samplerDescription.LODBias;

		emitter << YAML::EndMap;

		return true;
	}

	SharedRef<Resource> TextureSerializer::CreateResource(const ResourceID& id)
	{
		return CreateSharedRef<Texture>(id);
	}

	bool TextureSerializer::DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource)
	{
		SharedRef<Texture> texture = std::dynamic_pointer_cast<Texture>(resource);

		if (!texture)
		{
			CocoError("Resource was not a texture resource")
			return false;
		}

		FilePath imageFilePath(baseNode["imageFilePath"].as<string>());

		YAML::Node imageDescNode = baseNode["imageDescription"];
		texture->_imageDescription.Width = imageDescNode["width"].as<uint32>();
		texture->_imageDescription.Height = imageDescNode["height"].as<uint32>();
		texture->_imageDescription.Depth = imageDescNode["depth"].as<uint32>();
		texture->_imageDescription.Layers = imageDescNode["layers"].as<uint32>();
		texture->_imageDescription.PixelFormat = static_cast<ImagePixelFormat>(imageDescNode["pixelFormat"].as<int>());
		texture->_imageDescription.ColorSpace = static_cast<ImageColorSpace>(imageDescNode["colorSpace"].as<int>());
		texture->_imageDescription.MipCount = imageDescNode["mipmapCount"].as<uint32>();
		texture->_imageDescription.UsageFlags = static_cast<ImageUsageFlags>(imageDescNode["usageFlags"].as<int>());
		texture->_imageDescription.SampleCount = static_cast<MSAASamples>(imageDescNode["sampleCount"].as<int>());

		YAML::Node samplerDescNode = baseNode["samplerDescription"];
		ImageSamplerDescription samplerDescription(
			static_cast<ImageFilterMode>(samplerDescNode["minimizeFilter"].as<int>()),
			static_cast<ImageFilterMode>(samplerDescNode["magnifyFilter"].as<int>()),
			static_cast<ImageRepeatMode>(samplerDescNode["repeatU"].as<int>()),
			static_cast<ImageRepeatMode>(samplerDescNode["repeatV"].as<int>()),
			static_cast<ImageRepeatMode>(samplerDescNode["repeatW"].as<int>()),
			static_cast<MipMapFilterMode>(samplerDescNode["mipmapFilter"].as<int>()),
			samplerDescNode["maxAnisotropy"].as<uint32>(),
			samplerDescNode["minLod"].as<uint32>(),
			samplerDescNode["maxLod"].as<uint32>(),
			samplerDescNode["lodBias"].as<float>()
		);

		texture->LoadImage(imageFilePath);
		texture->CreateSampler(samplerDescription);

		texture->IncrementVersion();
		return true;
	}
}