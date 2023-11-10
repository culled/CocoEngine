#include "Corepch.h"
#include "ResourceLibrary.h"

#include "../Engine.h"

namespace Coco
{
	ResourceIDGenerator::ResourceIDGenerator() :
		Counter{}
	{}

	ResourceID ResourceIDGenerator::operator()()
	{
		return Counter++;
	}

	ResourceLibrary::~ResourceLibrary()
	{
		CocoTrace("Freeing {} resource(s)", _resources.size())

		_resources.clear();
	}

	bool ResourceLibrary::GetOrLoad(const FilePath& contentPath, bool forceReload, SharedRef<Resource>& outResource)
	{
		bool resourceLoaded = false;

		auto it = FindResource(contentPath);
		if (it != _resources.end())
		{
			outResource = it->second;
			resourceLoaded = true;
		}

		if (!forceReload && resourceLoaded)
			return true;

		EngineFileSystem& efs = Engine::Get()->GetFileSystem();
		if (!efs.FileExists(contentPath))
		{
			CocoError("File at \"{}\" does not exist", contentPath.ToString())
			return false;
		}

		ResourceSerializer* serializer = GetSerializerForFileType(contentPath);

		if (!serializer)
		{
			CocoError("No serializers support deserializing the file at \"{}\"", contentPath.ToString())
			return false;
		}

		File f = efs.OpenFile(contentPath, FileOpenFlags::Read | FileOpenFlags::Text);
		string data = f.ReadTextToEnd();
		f.Close();

		ResourceID id = _idGenerator();
		if (!resourceLoaded)
		{
			outResource = serializer->CreateAndDeserialize(id, data);
			_resources.try_emplace(id, outResource).first;
		}
		else
		{
			serializer->Deserialize(data, outResource);
		}

		// Loading into an existing resource makes it the same as the one on disk
		outResource->MarkSaved(efs.ConvertToShortPath(contentPath));

		CocoInfo("Loaded \"{}\"", contentPath.ToString())

		return true;
	}

	bool ResourceLibrary::Save(const FilePath& contentPath, SharedRef<Resource> resource, bool overwrite)
	{
		EngineFileSystem& efs = Engine::Get()->GetFileSystem();

		if (!overwrite && efs.FileExists(contentPath))
		{
			CocoError("Cannot overwrite existing file at \"{}\"", contentPath.ToString())
			return false;
		}

		ResourceSerializer* serializer = GetSerializerForFileType(contentPath);

		if (!serializer)
		{
			CocoError("No serializers support serializing the file at \"{}\"", contentPath.ToString())
			return false;
		}

		try
		{
			string data = serializer->Serialize(resource);

			File f = efs.OpenFile(contentPath, FileOpenFlags::Write | FileOpenFlags::Text);
			f.Write(data);
			f.Close();

			resource->MarkSaved(efs.ConvertToShortPath(contentPath));

			return true;
		}
		catch (const std::exception& ex)
		{
			CocoError("Error saving \"{}\": {}", contentPath.ToString(), ex.what())
			return false;
		}
	}

	bool ResourceLibrary::SaveAll(bool overwrite)
	{
		for (auto& kvp : _resources)
		{
			SharedRef<Resource> resource = kvp.second;

			const FilePath& fp = resource->GetContentPath();
			if (fp.IsEmpty() || !resource->NeedsSaving())
				continue;

			if (!Save(fp, resource, overwrite))
				return false;
		}

		return true;
	}

	bool ResourceLibrary::TryFindByPath(const FilePath& contentPath, SharedRef<Resource>& outResource) const
	{
		EngineFileSystem* efs = &Engine::Get()->GetFileSystem();

		ResourceMap::const_iterator it = std::find_if(_resources.begin(), _resources.end(),
			[efs, contentPath](const auto& kvp)
			{
				const SharedRef<Resource>& r = kvp.second;
				return efs->AreSameFile(r->_contentPath, contentPath);
			}
		);

		if (it == _resources.end())
			return false;

		outResource = it->second;
		return true;
	}

	ResourceSerializer* ResourceLibrary::GetSerializerForResourceType(const std::type_index& type)
	{
		auto it = std::find_if(_serializers.begin(), _serializers.end(), [type](const UniqueRef<ResourceSerializer>& serializer)
			{
				return serializer->SupportsResourceType(type);
			});

		if (it == _serializers.end())
			return nullptr;

		return it->get();
	}

	ResourceSerializer* ResourceLibrary::GetSerializerForFileType(const FilePath& contentPath)
	{
		string extension = contentPath.GetExtension();

		auto it = std::find_if(_serializers.begin(), _serializers.end(), [extension](const UniqueRef<ResourceSerializer>& serializer)
			{
				return serializer->SupportsFileExtension(extension);
			});

		if (it == _serializers.end())
			return nullptr;

		return it->get();
	}

	ResourceLibrary::ResourceMap::iterator ResourceLibrary::FindResource(const FilePath& contentPath)
	{
		return std::find_if(_resources.begin(), _resources.end(), [contentPath](const auto& kvp)
			{
				return kvp.second->_contentPath == contentPath;
			});
	}
}