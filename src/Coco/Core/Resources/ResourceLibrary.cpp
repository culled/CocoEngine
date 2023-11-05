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

	bool ResourceLibrary::GetOrLoad(const string& contentPath, bool forceReload, SharedRef<Resource>& outResource)
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
			CocoError("File at \"{}\" does not exist", contentPath)
			return false;
		}

		std::type_index resourceType = typeid(nullptr);
		ResourceSerializer* serializer = GetSerializerForFileType(contentPath, resourceType);

		if (!serializer)
		{
			CocoError("No serializers support deserializing the file at \"{}\"", contentPath)
			return false;
		}

		Assert(resourceType != typeid(nullptr))

		File f = efs.OpenFile(contentPath, FileOpenFlags::Read | FileOpenFlags::Text);
		string data = f.ReadTextToEnd();
		f.Close();

		ResourceID id = _idGenerator();
		SharedRef<Resource> newResource = serializer->Deserialize(resourceType, id, data);
		newResource->_contentPath = contentPath;

		if (resourceLoaded)
			Remove(outResource->_id);

		_resources.try_emplace(id, newResource).first;
		outResource = newResource;

		return true;
	}

	bool ResourceLibrary::Save(const string& contentPath, SharedRef<Resource> resource, bool overwrite)
	{
		EngineFileSystem& efs = Engine::Get()->GetFileSystem();

		if (!overwrite && efs.FileExists(contentPath))
		{
			CocoError("Cannot overwrite existing file at \"{}\"", contentPath)
			return false;
		}

		std::type_index resourceType = typeid(nullptr);
		ResourceSerializer* serializer = GetSerializerForFileType(contentPath, resourceType);

		if (!serializer)
		{
			CocoError("No serializers support serializing the file at \"{}\"", contentPath)
			return false;
		}

		try
		{
			string data = serializer->Serialize(resource);

			File f = efs.OpenFile(contentPath, FileOpenFlags::Write | FileOpenFlags::Text);
			f.Write(data);
			f.Close();

			resource->_contentPath = contentPath;

			return true;
		}
		catch (const std::exception& ex)
		{
			CocoError("Error saving \"{}\": {}", contentPath, ex.what())
			return false;
		}
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

	ResourceSerializer* ResourceLibrary::GetSerializerForFileType(const string& contentPath, std::type_index& outType)
	{
		FilePath fp(contentPath);
		string extension = fp.GetExtension();

		auto it = std::find_if(_serializers.begin(), _serializers.end(), [extension](const UniqueRef<ResourceSerializer>& serializer)
			{
				return serializer->SupportsFileExtension(extension);
			});

		if (it == _serializers.end())
			return nullptr;

		ResourceSerializer* serializer = it->get();
		outType = serializer->GetResourceTypeForExtension(extension);
		return serializer;
	}

	ResourceLibrary::ResourceMap::iterator ResourceLibrary::FindResource(const string& contentPath)
	{
		return std::find_if(_resources.begin(), _resources.end(), [contentPath](const auto& kvp)
			{
				return kvp.second->_contentPath == contentPath;
			});
	}
}