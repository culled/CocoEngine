#include "TextureLoader.h"

#include <Coco/Core/IO/File.h>
#include <Coco/Core/Resources/ResourceLibrary.h>
#include <Coco/Core/Logging/Logger.h>
#include "../Texture.h"

namespace Coco::Rendering
{
	TextureLoader::TextureLoader(ResourceLibrary* library) : KeyValueResourceLoader(library)
	{}

	Ref<Resource> TextureLoader::LoadImpl(const string& path)
	{
		ImageUsageFlags usageFlags = ImageUsageFlags::TransferSource | ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled;
		FilterMode filterMode = FilterMode::Linear;
		RepeatMode repeatMode = RepeatMode::Repeat;
		uint maxAnisotropy = 16;
		int channelCount = 4;
		string imageFilePath;
		string name;

		File file = File::Open(path, FileModeFlags::Read);
		KeyValueReader reader(file);
		while (reader.ReadLine())
		{
			if (reader.IsKey("version") && reader.GetValue() != "1")
				throw InvalidOperationException("Mismatching texture versions");
			else if (reader.IsKey(s_nameVariable))
				name = reader.GetValue();
			else if (reader.IsKey(s_imageFileVariable))
				imageFilePath = reader.GetValue();
			else if (reader.IsKey(s_usageFlagsVariable))
				usageFlags = static_cast<ImageUsageFlags>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_filterModeVariable))
				filterMode = static_cast<FilterMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_repeatModeVariable))
				repeatMode = static_cast<RepeatMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_maxAnisotropyVariable))
				maxAnisotropy = static_cast<uint>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_channelCountVariable))
				channelCount = reader.GetVariableValueAsInt();
		}

		if (imageFilePath.empty())
			LogWarning(Library->GetLogger(), FormattedString("\"{}\" did not have a valid image file", path));

		file.Close();

		return CreateRef<Texture>(imageFilePath, usageFlags, filterMode, repeatMode, maxAnisotropy, channelCount, name);
	}

	void TextureLoader::SaveImpl(const Ref<Resource>& resource, const string& path)
	{
		if (Texture* texture = dynamic_cast<Texture*>(resource.get()))
		{
			const ImageDescription textureDescription = texture->GetDescription();

			File file = File::Open(path, FileModeFlags::Write);
			KeyValueWriter writer(file);

			writer.WriteLine("version", "1");
			writer.WriteLine(s_nameVariable, texture->Name);
			writer.WriteLine(s_imageFileVariable, texture->GetImageFilePath());
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
