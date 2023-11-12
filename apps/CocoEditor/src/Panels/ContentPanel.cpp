#include "ContentPanel.h"

#include <Coco/Core/Engine.h>
#include <Coco/Core/IO/FileSystems/UnpackedEngineFileSystem.h>
#include <filesystem>

#include "../Importers/TextureImporter.h"
#include "../UI/UIUtils.h"

#include <Coco/Rendering/Material.h>

#include <imgui.h>

using namespace Coco::Rendering;

namespace Coco
{
	ContentPanel::ContentPanel() :
		_useTree(false),
		_showUnsupportedFiles(false),
		_fileTypeRegex("(.cscene)|(.cmaterial)|(.cmesh)|(.cshader)|(.ctexture)", std::regex_constants::ECMAScript | std::regex_constants::icase),
		_importers(),
		_materialDialog(CreateUniqueRef<MaterialResourceDialog>()),
		_meshDialog(CreateUniqueRef<MeshPrimitiveResourceDialog>())
	{
		if (UnpackedEngineFileSystem* fs = dynamic_cast<UnpackedEngineFileSystem*>(&Engine::Get()->GetFileSystem()))
			_basePath = fs->GetContentBasePath();
		else
			_basePath = FilePath::GetCurrentWorkingDirectoryPath();

		_currentPath = _basePath;

		auto& resources = Engine::Get()->GetResourceLibrary();

		_fileIcon = resources.Create<Texture>("File Icon", "ui/icons/file.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, ImageSamplerDescription::LinearClamp);
		_folderIcon = resources.Create<Texture>("File Icon", "ui/icons/folder.png", ImageColorSpace::sRGB, ImageUsageFlags::Sampled, ImageSamplerDescription::LinearClamp);

		_importers.push_back(CreateUniqueRef<TextureImporter>());
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

			// Right click over a black space
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
			{
				DrawPanelContextMenu();

				ImGui::EndPopup();
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

		string saveState = "";
		SharedRef<Resource> r;
		if (Engine::Get()->GetResourceLibrary().TryFindByPath(path, r) && r->NeedsSaving())
		{
			saveState = "*";
		}

		bool expanded = ImGui::TreeNodeEx(pathName.c_str(), flags, "%s%s", pathName.c_str(), saveState.c_str());

		if (!isDirectory)
		{
			if (ImGui::BeginDragDropSource())
			{
				DragDropFile(path);
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				OnFileDoubleClicked.Invoke(path);
			}
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

		auto& resources = Engine::Get()->GetResourceLibrary();

		for (const auto& it : std::filesystem::directory_iterator{ _currentPath })
		{
			FilePath fp(it.path());
			if (!FilterPath(fp) && !_showUnsupportedFiles)
				continue;

			ImGui::TableNextColumn();

			string pathName = fp.GetFileName(true);
			bool isDirectory = fp.IsDirectory();
			string saveState;;

			if (!isDirectory)
			{
				SharedRef<Resource> r;
				if (resources.TryFindByPath(fp, r) && r->NeedsSaving())
				{
					saveState = "*";
				}
			}

			ImGui::PushID(pathName.c_str());

			SharedRef<Texture> icon = isDirectory ? _folderIcon : _fileIcon;
			ImGui::ImageButton(icon.get(), thumbnailSize, ImVec2(0, 1), ImVec2(1, 0));

			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
				{
					_currentPath = fp;
				}
				else
				{
					OnFileDoubleClicked.Invoke(fp);
				}
			}

			if(!isDirectory && ImGui::BeginDragDropSource())
			{
				DragDropFile(fp);
				ImGui::EndDragDropSource();
			}

			ImGui::TextWrapped("%s%s", pathName.c_str(), saveState.c_str());

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

	void ContentPanel::DrawPanelContextMenu()
	{
		if (ImGui::MenuItem("Create Material"))
		{
			_materialDialog->SetSavePath(_currentPath);
			_materialDialog->Open();
		}

		if (ImGui::MenuItem("Create Mesh Primitive"))
		{
			_meshDialog->SetSavePath(_currentPath);
			_meshDialog->Open();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Import..."))
		{
			Engine* engine = Engine::Get();
			string path = engine->GetPlatform().ShowOpenFileDialog(
				{
					{"All supported files (*.png, *.jpg)", "*.png;*.jpg" }
				}
			);

			if (path.empty())
				return;

			Import(path, _currentPath);
		}
	}

	void ContentPanel::Import(const FilePath& filePath, const FilePath& saveDirectory)
	{
		string extension = filePath.GetExtension();

		auto it = std::find_if(_importers.begin(), _importers.end(),
			[filePath, extension](const UniqueRef<ResourceImporter>& importer)
			{
				return importer->SupportsExtension(extension);
			}
		);

		if (it == _importers.end())
		{
			CocoError("Unsupported file type: {}", extension)
			return;
		}

		ResourceImporter* importer = it->get();
		importer->Import(filePath, saveDirectory);

		CocoInfo("Imported {}", filePath.ToString())
	}
}