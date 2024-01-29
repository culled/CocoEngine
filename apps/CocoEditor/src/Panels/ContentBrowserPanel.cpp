#include "ContentBrowserPanel.h"

#include <Coco/Core/IO/FileSystems/UnpackedEngineFileSystem.h>
#include <Coco/Core/Engine.h>

#include "../Importers/Rendering/TextureImporter.h"

#include <imgui.h>

namespace Coco
{
	std::unordered_map<string, UniqueRef<AssetImporter>> ContentBrowserPanel::_importers;
	const std::regex ContentBrowserPanel::_fileTypeRegex = std::regex("(.cres)", std::regex_constants::ECMAScript | std::regex_constants::icase);

	ContentBrowserPanel::ContentBrowserPanel() :
		_showUnsupportedFiles(false)
	{
		if (UnpackedEngineFileSystem* fs = dynamic_cast<UnpackedEngineFileSystem*>(&Engine::Get()->GetFileSystem()))
		{
			_basePath = fs->GetContentBasePath();
		}
		else
		{
			_basePath = FilePath::GetCurrentWorkingDirectoryPath();
		}

		_currentPath = _basePath;
		UpdateDatabaseForDirectory(_currentPath);

		RegisterImporter<TextureImporter>(".png");
	}

	void ContentBrowserPanel::Draw()
	{
		if (ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_MenuBar))
		{
			DrawMenu();
			DrawCurrentPath();
		}

		ImGui::End();
	}

	void ContentBrowserPanel::SetCurrentPath(const FilePath& newPath)
	{
		_currentPath = newPath;
		UpdateDatabaseForDirectory(newPath);
	}

	void ContentBrowserPanel::RefreshCurrentPath()
	{
		UpdateDatabaseForDirectory(_currentPath);
	}

	void ContentBrowserPanel::DrawMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::Checkbox("Show Unsupported File Types", &_showUnsupportedFiles);

			ImGui::EndMenuBar();
		}
	}

	bool ContentBrowserPanel::FilterPath(const FilePath& filePath)
	{
		if (filePath.IsDirectory())
			return true;

		string ext = filePath.GetExtension();

		return std::regex_search(ext, _fileTypeRegex);
	}

	void ContentBrowserPanel::DrawCurrentPath()
	{
		if (_currentPath != _basePath)
		{
			if (ImGui::Button("Back"))
			{
				_currentPath = _currentPath.GetParentDirectory();
			}

			ImGui::SameLine();
		}

		FilePath relativePath = FilePath::GetRelativePath(_currentPath, _basePath);
		string relativePathName = relativePath.ToString();
		ImGui::Text("%s", relativePathName.c_str());

		// TODO: configurable thumbnail size & padding
		ImVec2 thumbnailSize(80, 80);
		float padding = 16.f;
		ImVec2 cellSize(thumbnailSize.x + padding, thumbnailSize.y + padding);

		float width = ImGui::GetContentRegionAvail().x;
		int columns = static_cast<int>(Math::Floor(width / cellSize.x));

		if (columns <= 0)
		{
			columns = 1;
			cellSize = ImVec2(width, width);
		}

		if (!ImGui::BeginTable("Contents", columns, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame))
			return;

		for (const auto& it : std::filesystem::directory_iterator{ _currentPath })
		{
			
			FilePath fp(it.path());
			string pathName = fp.GetFileName(true);
			bool isDirectory = fp.IsDirectory();

			if (!_showUnsupportedFiles && !FilterPath(fp))
				continue;

			ImGui::TableNextColumn();

			ImGui::PushID(pathName.c_str());

			ImGui::Button(pathName.c_str(), thumbnailSize);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
				{
					SetCurrentPath(fp);
				}
				else
				{
					// TODO: do something with the file
				}
			}

			if (!isDirectory && ImGui::BeginDragDropSource())
			{
				DragDropFile(fp);
				ImGui::EndDragDropSource();
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	void ContentBrowserPanel::DragDropFile(const FilePath& filePath)
	{
		if (!_resourceTypeDatabase.contains(filePath))
		{
			CocoError("Could not determine resource type for file {}", filePath.ToString())
			return;
		}

		string resourceType = _resourceTypeDatabase[filePath];

		FilePath relative = FilePath::GetRelativePath(filePath, _basePath);
		string itemPath = relative.ToString();
		ImGui::SetDragDropPayload(resourceType.c_str(), itemPath.c_str(), itemPath.size(), ImGuiCond_Once);
	}

	void ContentBrowserPanel::UpdateDatabaseForDirectory(const FilePath& directory)
	{
		AssetManager& assets = Engine::Get()->GetAssetManager();

		for (const auto& it : std::filesystem::directory_iterator{ directory })
		{
			FilePath fp(it.path());
			string extension = fp.GetExtension();

			if (fp.IsDirectory())
				continue;

			// TODO: binary resources
			if (extension == AssetManager::TextAssetExtension)
			{
				_resourceTypeDatabase[fp] = assets.GetAssetResourceType(fp);
			}
			else
			{
				ImportFile(fp);
			}
		}
	}

	void ContentBrowserPanel::ImportFile(const FilePath& filePath)
	{
		FilePath newPath = filePath.ChangeExtension("cres");

		if (Engine::Get()->GetFileSystem().FileExists(newPath))
			return;

		string ext = filePath.GetExtension();
		auto it = _importers.find(ext);

		if (it == _importers.end())
			return;

		SharedRef<Resource> res = it->second->Import(filePath);
		if (!res)
			return;

		Engine::Get()->GetAssetManager().Save(res, newPath);
		_resourceTypeDatabase[filePath] = res->GetTypename();

		CocoInfo("Imported \"{}\" as \"{}\"", filePath.ToString(), newPath.ToString())
	}
}