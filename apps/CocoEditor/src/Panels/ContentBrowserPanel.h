#pragma once

#include <unordered_map>
#include <Coco/Core/IO/FilePath.h>
#include <Coco/Core/Types/Refs.h>
#include "../Importers/AssetImporter.h"

namespace Coco
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		template<typename ImporterType>
		static void RegisterImporter(const string& fileExtension)
		{
			_importers.try_emplace(fileExtension, CreateUniqueRef<ImporterType>());
		}

		void Draw();
		void SetCurrentPath(const FilePath& newPath);
		void RefreshCurrentPath();

	private:
		static std::unordered_map<string, UniqueRef<AssetImporter>> _importers;
		static const std::regex _fileTypeRegex;

		FilePath _basePath;
		FilePath _currentPath;
		bool _showUnsupportedFiles;

		std::unordered_map<FilePath, string> _resourceTypeDatabase;

	private:
		void DrawMenu();
		bool FilterPath(const FilePath& filePath);
		void DrawCurrentPath();
		void DragDropFile(const FilePath& filePath);

		void UpdateDatabaseForDirectory(const FilePath& directory);
		void ImportFile(const FilePath& filePath);
	};
}