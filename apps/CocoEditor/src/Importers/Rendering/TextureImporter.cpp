#include "TextureImporter.h"

#include <Coco/Rendering/Texture.h>

using namespace Coco::Rendering;

namespace Coco
{
	SharedRef<Resource> TextureImporter::Import(const FilePath& filePath)
	{
		return CreateSharedRef<Texture>(
			ResourceID(filePath.ToString()),
			filePath,
			ImageColorSpace::sRGB,
			ImageUsageFlags::Sampled,
			ImageSamplerDescription::LinearRepeat
		);
	}
}