#include "Corepch.h"
#include "AssetManager.h"
#include "../Engine.h"
#include "ResourceSerializer.h"
#include <yaml-cpp/yaml.h>

namespace Coco
{
	YAML::Node LoadAssetYAML(const FilePath& filePath)
	{
		// Read-in the file as text
		File file = Engine::Get()->GetFileSystem().OpenFile(filePath, FileOpenFlags::Read | FileOpenFlags::Text);

		string text = file.ReadTextToEnd();
		file.Close();

		return YAML::Load(text);
	}

	const string AssetManager::TextAssetExtension = ".cres";

	SharedRef<Resource> AssetManager::GetOrLoad(const FilePath& filePath)
	{
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();
		SharedRef<Resource> resource;

		// Try to find the ID of the resource associated with the file path
		auto it = _resourcePathMap.find(filePath);

		if (it != _resourcePathMap.end())
		{
			const ResourceID& id = it->second;

			// If the resource has already been loaded, just return that
			if (resources.TryGet(id, resource))
				return resource;
		}
		
		// The resource has not been loaded, so we should load it
		string extension = filePath.GetExtension();
		bool isTextAsset = extension == TextAssetExtension;

		bool loaded = false;

		if (isTextAsset)
		{
			loaded = LoadTextAsset(filePath, resource);
		}
		else
		{
			// TODO: load binary asset
			throw NotImplementedException("Only text assets can be loaded currently");
		}

		if (loaded)
		{
			resources.Add(resource);

			_resourcePathMap[filePath] = resource->GetID();
			CocoInfo("Loaded resource {} from \"{}\"", resource->GetID().ToString(), filePath.ToString())
		}

		return resource;
	}

	bool AssetManager::Save(const SharedRef<Resource>& resource, const FilePath& filePath)
	{
		string extension = filePath.GetExtension();
		bool saveAsText = extension == TextAssetExtension;
		bool saved = false;

		if (saveAsText)
		{
			saved = SaveTextAsset(resource, filePath);
		}
		else
		{
			// TODO: save binary asset
			throw NotImplementedException("Only text assets can be saved currently");
		}

		CocoInfo("Saved resource {} to \"{}\"", resource->GetID().ToString(), filePath.ToString())

		_resourcePathMap[filePath] = resource->GetID();
		return saved;
	}

	FilePath AssetManager::GetResourceFilePath(const ResourceID& id) const
	{
		for (const auto& kvp : _resourcePathMap)
		{
			if (kvp.second == id)
				return kvp.first;
		}

		return FilePath();
	}

	string AssetManager::GetAssetResourceType(const FilePath& assetPath)
	{
		ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

		// Try to find the ID of the resource associated with the file path
		auto it = _resourcePathMap.find(assetPath);
		SharedRef<Resource> resource;

		if (it != _resourcePathMap.end())
		{
			const ResourceID& id = it->second;

			// If the resource has already been loaded, just return that
			if (resources.TryGet(id, resource))
				return resource->GetTypename();
		}

		// The resource has not been loaded, so we should load it
		string extension = assetPath.GetExtension();
		bool isTextAsset = extension == TextAssetExtension;

		bool loaded = false;

		if (isTextAsset)
		{
			YAML::Node baseNode = LoadAssetYAML(assetPath);

			if (!baseNode["resourceType"])
			{
				CocoError("Resource format is incorrect")
				return string();
			}

			return baseNode["resourceType"].as<string>();
		}
		else
		{
			// TODO: load binary asset
			throw NotImplementedException("Only text assets can be loaded currently");
			return string();
		}
	}

	bool AssetManager::LoadTextAsset(const FilePath& filePath, SharedRef<Resource>& outResource)
	{
		YAML::Node baseNode = LoadAssetYAML(filePath);

		if (!baseNode["resourceID"] || !baseNode["resourceType"])
		{
			CocoError("Resource format is incorrect")
			return false;
		}

		ResourceID resourceID(baseNode["resourceID"].as<string>());
		string resourceType = baseNode["resourceType"].as<string>();
		UniqueRef<ResourceSerializer> serializer = ResourceSerializerFactory::Create(resourceType);

		if (!serializer)
		{
			CocoError("No serializer has been registered for resource type \"{}\"", resourceType)
			return false;
		}

		outResource = serializer->CreateResource(resourceID);
		return serializer->DeserializeYAML(baseNode, outResource);
	}

	bool AssetManager::SaveTextAsset(const SharedRef<Resource>& resource, const FilePath& filePath)
	{
		YAML::Emitter emitter;
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "resourceID" << YAML::Value << resource->GetID().ToString();
		emitter << YAML::Key << "resourceType" << YAML::Value << resource->GetTypename();

		string resourceType = resource->GetTypename();
		UniqueRef<ResourceSerializer> serializer = ResourceSerializerFactory::Create(resourceType);
		
		if (!serializer->SerializeYAML(emitter, resource))
			return false;

		// HACK: without an ending character, loading YAML seems to fail
		emitter << YAML::EndMap << YAML::Comment(".");

		string data = emitter.c_str();
		File file = Engine::Get()->GetFileSystem().OpenFile(filePath, FileOpenFlags::Write | FileOpenFlags::Text, true);
		file.Write(data);
		file.Close();

		return true;
	}
}