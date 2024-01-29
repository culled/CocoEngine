#pragma once
#include "../Corepch.h"
#include "Resource.h"
#include "../IO/FilePath.h"

namespace Coco
{
	class AssetManager
	{
	public:
		static const string TextAssetExtension;

	public:
		SharedRef<Resource> GetOrLoad(const FilePath& filePath);

		template<typename ResourceType>
		SharedRef<ResourceType> GetOrLoadAs(const FilePath& filePath)
		{
			SharedRef<Resource> r = GetOrLoad(filePath);

			if (!r && r->GetType() != typeid(ResourceType))
				return SharedRef<ResourceType>();

			return std::static_pointer_cast<ResourceType>(r);
		}

		bool Save(const SharedRef<Resource>& resource, const FilePath& filePath);
		FilePath GetResourceFilePath(const ResourceID& id) const;

		string GetAssetResourceType(const FilePath& assetPath);

	private:
		std::unordered_map<FilePath, ResourceID> _resourcePathMap;

	private:
		bool LoadTextAsset(const FilePath& filePath, SharedRef<Resource>& resource);
		bool SaveTextAsset(const SharedRef<Resource>& resource, const FilePath& filePath);
	};
}