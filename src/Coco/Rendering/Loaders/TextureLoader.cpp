#include "TextureLoader.h"

#include <Coco/Core/IO/File.h>
#include "../Texture.h"

namespace Coco::Rendering
{
	TextureLoader::TextureLoader(const string& basePath) : ResourceLoader(basePath)
	{}

	Ref<Resource> TextureLoader::LoadImpl(const string& path)
	{
		const string& filePath = BasePath + path;

		ImageUsageFlags usageFlags = ImageUsageFlags::TransferSource | ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled;
		FilterMode filterMode = FilterMode::Linear;
		RepeatMode repeatMode = RepeatMode::Repeat;
		uint maxAnisotropy = 16;
		int channelCount = 4;
		string imageFilePath;

		File file = File::Open(filePath, FileModes::Read);
		string line;
		while (file.ReadLine(line))
		{
			size_t delimiter = line.find('=');

			if (delimiter == string::npos)
				continue;

			string variable = line.substr(0, delimiter);
			string value = line.substr(delimiter + 1);

			if (variable == "version" && value != "1")
				throw InvalidOperationException("Mismatching texture versions");
			else if (variable == "imageFile")
				imageFilePath = value;
			else if (variable == "usageFlags")
				usageFlags = static_cast<ImageUsageFlags>(atoi(value.c_str()));
			else if (variable == "filterMode")
				filterMode = static_cast<FilterMode>(atoi(value.c_str()));
			else if (variable == "repeatMode")
				repeatMode = static_cast<RepeatMode>(atoi(value.c_str()));
			else if (variable == "maxAnisotropy")
				maxAnisotropy = static_cast<uint>(atoi(value.c_str()));
			else if (variable == "channelCount")
				channelCount = atoi(value.c_str());
		}

		file.Close();

		return CreateRef<Texture>(imageFilePath, usageFlags, filterMode, repeatMode, maxAnisotropy, channelCount);
	}

	void TextureLoader::SaveImpl(const Ref<Resource>& resource, const string& path)
	{
		if (Texture* texture = dynamic_cast<Texture*>(resource.get()))
		{
			const string& filePath = BasePath + path;
			const ImageDescription textureDescription = texture->GetDescription();

			File file = File::Open(filePath, FileModes::Write);

			file.WriteLine("version=1");
			file.WriteLine(FormattedString("usageFlags={}", static_cast<uint>(textureDescription.UsageFlags)));
			file.WriteLine(FormattedString("filterMode={}", static_cast<uint>(texture->GetFilterMode())));
			file.WriteLine(FormattedString("repeatMode={}", static_cast<uint>(texture->GetRepeatMode())));
			file.WriteLine(FormattedString("maxAnisotropy={}", texture->GetMaxAnisotropy()));
			file.WriteLine(FormattedString("channelCount={}", GetPixelFormatChannelCount(textureDescription.PixelFormat)));

			file.Close();
		}
		else
		{
			throw InvalidOperationException("Resource was not a texture");
		}
	}
}
