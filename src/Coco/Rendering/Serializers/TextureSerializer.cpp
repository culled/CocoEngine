#include "Renderpch.h"
#include "TextureSerializer.h"

#include "../Texture.h"
#include "Types/ImageDescriptionSerializer.h"
#include "Types/ImageSamplerDescriptionSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Coco::Rendering
{
	bool TextureSerializer::SupportsFileExtension(const string& extension) const
	{
		return extension == ".ctexture";
	}

	bool TextureSerializer::SupportsResourceType(const std::type_index& type) const
	{
		return type == typeid(Texture);
	}

	string TextureSerializer::Serialize(Ref<Resource> resource)
	{
		const Texture* texture = dynamic_cast<const Texture*>(resource.Get());
		Assert(texture)
		Assert(texture->GetImage().IsValid())
		Assert(texture->GetImageSampler().IsValid())

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "name" << YAML::Value << texture->GetName();
		out << YAML::Key << "image file" << YAML::Value << texture->_imageFilePath;

		out << YAML::Key << "image description" << YAML::Value << YAML::BeginMap;
		ImageDescriptionSerializer::Serialize(out, texture->GetImage()->GetDescription());
		out << YAML::EndMap;

		out << YAML::Key << "sampler description" << YAML::Value << YAML::BeginMap;
		ImageSamplerDescriptionSerializer::Serialize(out, texture->GetImageSampler()->GetDescription());
		out << YAML::EndMap;

		out << YAML::EndMap << YAML::Comment("Fix");

		return string(out.c_str());
	}

	ManagedRef<Resource> TextureSerializer::Deserialize(const ResourceID& resourceID, const string& data)
	{
		YAML::Node baseNode = YAML::Load(data);
		string name = baseNode["name"].as<string>();
		string imageFilePath = baseNode["image file"].as<string>();

		YAML::Node imageDescNode = baseNode["image description"];
		ImageDescription imageDesc = ImageDescriptionSerializer::Deserialize(imageDescNode);

		YAML::Node samplerDescNode = baseNode["sampler description"];
		ImageSamplerDescription samplerDesc = ImageSamplerDescriptionSerializer::Deserialize(samplerDescNode);

		return CreateManagedRef<Texture>(resourceID, name, imageFilePath, imageDesc.ColorSpace, imageDesc.UsageFlags, samplerDesc);
	}
}