#include "TextureImporter.h"

#include <Coco/Rendering/Texture.h>
#include <Coco/Core/Engine.h>

using namespace Coco::Rendering;

namespace Coco
{
	bool TextureImporter::SupportsExtension(const string& extension)
	{
		return extension == ".png" || extension == ".jpg";
	}

	SharedRef<Resource> TextureImporter::Import(const FilePath& path, std::optional<FilePath> saveDirectory)
	{
		FilePath shortImagePath = Engine::Get()->GetFileSystem().ConvertToShortPath(path);

		auto& resources = Engine::Get()->GetResourceLibrary();

		string name = path.GetFileName(false);
		SharedRef<Texture> tex = resources.Create<Texture>(
			name,
			shortImagePath,
			ImageColorSpace::sRGB, 
			ImageUsageFlags::Sampled, 
			ImageSamplerDescription::LinearRepeat
		);

		if (saveDirectory.has_value())
		{
			FilePath contentPath = saveDirectory.value() / FilePath(name + ".ctexture");
			resources.Save(contentPath, tex, true);
		}

		return tex;
	}
}