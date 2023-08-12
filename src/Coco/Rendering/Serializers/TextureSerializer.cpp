#include "TextureSerializer.h"

#include <Coco/Core/IO/File.h>
#include <Coco/Core/Resources/ResourceLibrary.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Types/UUID.h>

#include <sstream>

namespace Coco::Rendering
{
	string TextureSerializer::Serialize(ResourceLibrary& library, const Ref<Resource>& resource)
	{
		if (const Texture* texture = dynamic_cast<const Texture*>(resource.Get()))
		{
			//const ImageDescription textureDescription = texture->GetDescription();
			//
			//KeyValueWriter writer;
			//
			//writer.WriteLine("version", "1");
			//writer.WriteLine(s_nameVariable, texture->GetName());
			//writer.WriteLine(s_imageFileVariable, texture->GetImageFilePath());
			//writer.WriteLine(s_usageFlagsVariable, ToString(static_cast<uint>(textureDescription.UsageFlags)));
			//writer.WriteLine(s_filterModeVariable, ToString(static_cast<uint>(texture->GetFilterMode())));
			//writer.WriteLine(s_repeatModeVariable, ToString(static_cast<uint>(texture->GetRepeatMode())));
			//writer.WriteLine(s_maxAnisotropyVariable, ToString(texture->GetMaxAnisotropy()));
			//writer.WriteLine(s_channelCountVariable, ToString(GetPixelFormatChannelCount(textureDescription.PixelFormat)));
			//
			//return writer.Flush();
			return "";
		}
		else
		{
			throw InvalidOperationException("Resource was not a texture");
		}
	}

	ManagedRef<Resource> TextureSerializer::Deserialize(ResourceLibrary& library, const string& data)
	{
		ImageUsageFlags usageFlags = ImageUsageFlags::TransferSource | ImageUsageFlags::TransferDestination | ImageUsageFlags::Sampled;
		ImageFilterMode filterMode = ImageFilterMode::Linear;
		ImageRepeatMode repeatMode = ImageRepeatMode::Repeat;
		uint maxAnisotropy = 16;
		ColorSpace colorSpace = ColorSpace::sRGB;
		string imageFilePath;
		string name;
		string id;

		std::stringstream stream(data);
		KeyValueReader reader(stream);
		while (reader.ReadLine())
		{
			if (reader.IsKey("version") && reader.GetValue() != "1")
				throw InvalidOperationException("Mismatching texture versions");
			else if (reader.IsKey(s_idVariable))
				id = reader.GetValue();
			else if (reader.IsKey(s_nameVariable))
				name = reader.GetValue();
			else if (reader.IsKey(s_imageFileVariable))
				imageFilePath = reader.GetValue();
			else if (reader.IsKey(s_usageFlagsVariable))
				usageFlags = static_cast<ImageUsageFlags>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_filterModeVariable))
				filterMode = static_cast<ImageFilterMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_repeatModeVariable))
				repeatMode = static_cast<ImageRepeatMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_maxAnisotropyVariable))
				maxAnisotropy = static_cast<uint>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_colorSpaceVariable))
				colorSpace = static_cast<ColorSpace>(reader.GetVariableValueAsInt());
		}

		if (imageFilePath.empty())
			LogWarning(library.GetLogger(), "Texture did not have a valid image file");

		ImageSamplerProperties samplerProperties = ImageSamplerProperties(filterMode, repeatMode, maxAnisotropy);
		ManagedRef<Texture> texture = CreateManagedRef<Texture>(UUID(id), name, library.GetFullFilePath(imageFilePath), colorSpace, usageFlags, samplerProperties);

		return std::move(texture);
	}
}
