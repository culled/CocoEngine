#include "TextureLoader.h"

#include <Coco/Core/IO/File.h>
#include "../Texture.h"

namespace Coco::Rendering
{
	TextureLoader::TextureLoader(const string& basePath) : KeyValueResourceLoader(basePath)
	{}

	Ref<Resource> TextureLoader::LoadImpl(const string& path)
	{
		ImageUsageFlags usageFlags = ImageUsageFlags::TransferSource | ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled;
		FilterMode filterMode = FilterMode::Linear;
		RepeatMode repeatMode = RepeatMode::Repeat;
		uint maxAnisotropy = 16;
		int channelCount = 4;
		string imageFilePath;

		File file = File::Open(path, FileModes::Read);
		KeyValueReader reader(file);
		while (reader.ReadLine())
		{
			if (reader.IsVariable("version") && reader.GetVariableValue() != "1")
				throw InvalidOperationException("Mismatching texture versions");
			else if (reader.IsVariable(s_imageFileVariable))
				imageFilePath = reader.GetVariableValue();
			else if (reader.IsVariable(s_usageFlagsVariable))
				usageFlags = static_cast<ImageUsageFlags>(reader.GetVariableValueAsInt());
			else if (reader.IsVariable(s_filterModeVariable))
				filterMode = static_cast<FilterMode>(reader.GetVariableValueAsInt());
			else if (reader.IsVariable(s_repeatModeVariable))
				repeatMode = static_cast<RepeatMode>(reader.GetVariableValueAsInt());
			else if (reader.IsVariable(s_maxAnisotropyVariable))
				maxAnisotropy = static_cast<uint>(reader.GetVariableValueAsInt());
			else if (reader.IsVariable(s_channelCountVariable))
				channelCount = reader.GetVariableValueAsInt();
		}

		file.Close();

		return CreateRef<Texture>(imageFilePath, usageFlags, filterMode, repeatMode, maxAnisotropy, channelCount);
	}

	void TextureLoader::SaveImpl(const Ref<Resource>& resource, const string& path)
	{
		if (Texture* texture = dynamic_cast<Texture*>(resource.get()))
		{
			const ImageDescription textureDescription = texture->GetDescription();

			File file = File::Open(path, FileModes::Write);
			KeyValueWriter writer(file);

			writer.WriteLine("version", "1");
			writer.WriteLine(s_usageFlagsVariable, ToString(static_cast<uint>(textureDescription.UsageFlags)));
			writer.WriteLine(s_filterModeVariable, ToString(static_cast<uint>(texture->GetFilterMode())));
			writer.WriteLine(s_repeatModeVariable, ToString(static_cast<uint>(texture->GetRepeatMode())));
			writer.WriteLine(s_maxAnisotropyVariable, ToString(texture->GetMaxAnisotropy()));
			writer.WriteLine(s_channelCountVariable, ToString(GetPixelFormatChannelCount(textureDescription.PixelFormat)));

			file.Close();
		}
		else
		{
			throw InvalidOperationException("Resource was not a texture");
		}
	}
}
