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

	bool ResourceLibrary::GetOrLoad(const string& contentPath, Ref<Resource>& outResource)
	{
		auto it = FindResource(contentPath);
		if (it != _resources.end())
		{
			outResource = it->second;
			return true;
		}

		EngineFileSystem* efs = Engine::Get()->GetFileSystem();

		if (!efs->FileExists(contentPath))
		{
			CocoError("File at \"{}\" does not exist", contentPath)
			return false;
		}

		ResourceSerializer* serializer = GetSerializerForFileType(contentPath);

		if (!serializer)
		{
			CocoError("No serializers support deserializing the file at \"{}\"", contentPath)
				return false;
		}

		File f = efs->OpenFile(contentPath, FileOpenFlags::Read);
		std::vector<uint8> data = f.ReadToEnd();
		f.Close();

		ResourceID id = _idGenerator();
		it = _resources.try_emplace(id, serializer->Deserialize(id, data)).first;
		
		ManagedRef<Resource>& resource = it->second;
		resource->_contentPath = contentPath;
		outResource = resource;

		return true;
	}

	bool ResourceLibrary::Save(const string& contentPath, Ref<Resource> resource, bool overwrite)
	{
		EngineFileSystem* efs = Engine::Get()->GetFileSystem();

		if (!overwrite && efs->FileExists(contentPath))
		{
			CocoError("Cannot overwrite existing file at \"{}\"", contentPath)
			return false;
		}

		ResourceSerializer* serializer = GetSerializerForFileType(contentPath);

		if (!serializer)
		{
			CocoError("No serializers support serializing the file at \"{}\"", contentPath)
			return false;
		}

		try
		{
			std::vector<uint8> data = serializer->Serialize(resource);

			File f = efs->OpenFile(contentPath, FileOpenFlags::Write);
			f.Write(data);
			f.Close();

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

	ResourceSerializer* ResourceLibrary::GetSerializerForFileType(const string& contentPath)
	{
		FilePath fp(contentPath);
		string extension = fp.GetExtension();

		auto it = std::find_if(_serializers.begin(), _serializers.end(), [extension](const UniqueRef<ResourceSerializer>& serializer)
			{
				return serializer->SupportsFileExtension(extension);
			});

		if (it == _serializers.end())
			return nullptr;

		return it->get();
	}

	ResourceLibrary::ResourceMap::iterator ResourceLibrary::FindResource(const string& contentPath)
	{
		return std::find_if(_resources.begin(), _resources.end(), [contentPath](const auto& kvp)
			{
				return kvp.second->_contentPath == contentPath;
			});
	}
}