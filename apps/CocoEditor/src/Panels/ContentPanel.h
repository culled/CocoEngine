#pragma once

#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/Core/IO/FileTypes.h>
#include <Coco/Core/Events/Event.h>

#include <Coco/Rendering/Texture.h>

namespace Coco
{
	class ContentPanel
	{
	public:
		Event<const FilePath&> OnFileDoubleClicked;

	private:
		bool _useTree;
		bool _showUnsupportedFiles;
		FilePath _basePath;
		FilePath _currentPath;

		std::regex _fileTypeRegex;
		SharedRef<Rendering::Texture> _fileIcon;
		SharedRef<Rendering::Texture> _folderIcon;

	public:
		ContentPanel();

		void Update(const TickInfo& tick);

	private:
		void DrawMenu();

		void DrawFileTreeNode(const FilePath& path);
		void DrawCurrentPath();

		void DragDropFile(const FilePath& filePath);

		bool FilterPath(const FilePath& filePath);
	};
}