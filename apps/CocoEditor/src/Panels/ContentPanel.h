#pragma once

#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/Core/IO/FileTypes.h>

#include <Coco/Rendering/Texture.h>

namespace Coco
{
	class ContentPanel
	{
	private:
		bool _useTree;
		FilePath _basePath;
		FilePath _currentPath;

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
	};
}