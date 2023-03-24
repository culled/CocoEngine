#include "ResourceLoader.h"

#include "ResourceLibrary.h"

namespace Coco
{
	ResourceLoader::ResourceLoader(ResourceLibrary* library) : Library(library)
	{}

	Ref<Resource> ResourceLoader::Load(const string & path)
	{
		Ref<Resource> resource = LoadImpl(Library->GetFullFilePath(path));
		resource->SetFilePath(path);

		return resource;
	}

	void ResourceLoader::Save(const Ref<Resource>& resource, const string& path)
	{
		SaveImpl(resource, Library->GetFullFilePath(path));
		resource->SetFilePath(path);
	}
}
