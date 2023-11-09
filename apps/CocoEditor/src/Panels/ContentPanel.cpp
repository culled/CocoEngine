#include "ContentPanel.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/IO/FileSystems/UnpackedEngineFileSystem.h>
#include <filesystem>

#include <imgui.h>

using namespace Coco::Rendering;

namespace Coco
{
	ContentPanel::ContentPanel() :
		_useTree(false),
		_showUnsupportedFiles(false),
		_fileTypeRegex("(.cscene)|(.cmaterial)|(.cmesh)|(.cshader)|(.ctexture)", std::regex_constants::ECMAScript | std::regex_constants::icase)
	{
		if (UnpackedEngineFileSystem* fs = dynamic_cast<UnpackedEngineFileSystem*>(&Engine::Get()->GetFileSystem()))
			_basePath = fs->GetContentBasePath();
		else
			_basePath = FilePath::GetCurrentWorkingDirectoryPath();

		_currentPath = _basePath;

		auto& resources = Engine::Get()->GetResourceLibrary();

		_fileIcon = resources.Create<Texture>("File Icon", "ui/icons/file.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, ImageSamplerDescription::LinearClamp);
		_folderIcon = resources.Create<Texture>("File Icon", "ui/icons/folder.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, ImageSamplerDescription::LinearClamp);
	}

	void ContentPanel::Update(const TickInfo& tick)
	{
		if (ImGui::Begin("Content", nullptr, ImGuiWindowFlags_MenuBar))
		{
			DrawMenu();

			if (UnpackedEngineFileSystem* fs = dynamic_cast<UnpackedEngineFileSystem*>(&Engine::Get()->GetFileSystem()))
			{
				if (_useTree)
				{
					FilePath basePath = fs->GetContentBasePath();
					DrawFileTreeNode(basePath);
				}
				else
				{
					DrawCurrentPath();
				}
			}
		}

		ImGui::End();
	}

	void ContentPanel::DrawMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::Checkbox("Tree View", &_useTree);

			ImGui::Checkbox("Show Unsupported File Types", &_showUnsupportedFiles);

			ImGui::EndMenuBar();
		}
	}

	void ContentPanel::DrawFileTreeNode(const FilePath& path)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool isDirectory = path.IsDirectory();
		if (!isDirectory)
			flags |= ImGuiTreeNodeFlags_Leaf;

		string pathName = path.GetFileName(true);
		bool expanded = ImGui::TreeNodeEx(pathName.c_str(), flags, "%s", pathName.c_str());

		if (!isDirectory && ImGui::BeginDragDropSource())
		{
			DragDropFile(path);
			ImGui::EndDragDropSource();
		}

		if (expanded)
		{
			if (isDirectory)
			{
				for (const auto& it : std::filesystem::directory_iterator{ path })
				{
					FilePath fp(it.path());

					if(FilterPath(fp) || _showUnsupportedFiles)
						DrawFileTreeNode(fp);
				}
			}

			ImGui::TreePop();
		}
	}

	void ContentPanel::DrawCurrentPath()
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
			if (!FilterPath(fp) && !_showUnsupportedFiles)
				continue;

			ImGui::TableNextColumn();

			string pathName = fp.GetFileName(true);
			bool isDirectory = fp.IsDirectory();

			ImGui::PushID(pathName.c_str());

			SharedRef<Texture> icon = isDirectory ? _folderIcon : _fileIcon;
			ImGui::ImageButton(icon.get(), thumbnailSize, ImVec2(0, 1), ImVec2(1, 0));

			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
				{
					_currentPath = fp;
				}
			}

			if(!isDirectory && ImGui::BeginDragDropSource())
			{
				DragDropFile(fp);
				ImGui::EndDragDropSource();
			}

			ImGui::TextWrapped("%s", pathName.c_str());

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	void ContentPanel::DragDropFile(const FilePath& filePath)
	{
		FilePath relative = FilePath::GetRelativePath(filePath, _basePath);
		string itemPath = relative.ToString();
		string itemType = relative.GetExtension();
		ImGui::SetDragDropPayload(itemType.c_str(), itemPath.c_str(), itemPath.size(), ImGuiCond_Once);
	}

	bool ContentPanel::FilterPath(const FilePath& filePath)
	{
		if (filePath.IsDirectory())
			return true;

		string ext = filePath.GetExtension();

		return std::regex_search(ext, _fileTypeRegex);
	}
}