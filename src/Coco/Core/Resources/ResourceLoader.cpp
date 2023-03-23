#include "ResourceLoader.h"

#include "ResourceLibrary.h"

namespace Coco
{
	ResourceLoader::ResourceLoader(ResourceLibrary* library, const string& basePath) : Library(library), BasePath(basePath)
	{}

	Ref<Resource> ResourceLoader::Load(const string & path)
	{
		Ref<Resource> resource = LoadImpl(BasePath + path);
		resource->SetFilePath(path);

		return resource;
	}

	void ResourceLoader::Save(const Ref<Resource>& resource, const string& path)
	{
		SaveImpl(resource, BasePath + path);
		resource->SetFilePath(path);
	}
}
