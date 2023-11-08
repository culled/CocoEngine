#include "Corepch.h"
#include "UnpackedEngineFileSystem.h"

#include "../../Engine.h"

namespace Coco
{
	UnpackedEngineFileSystem::UnpackedEngineFileSystem(const FilePath& contentBasePath) :
		_contentBasePath(contentBasePath)
	{
		CocoTrace("Initialized UnpackedEngineFileSystem with content path at \"{}\"", contentBasePath.ToString())
	}

	File UnpackedEngineFileSystem::OpenFile(const FilePath& contentPath, FileOpenFlags openFlags)
	{
		return File(GetFullFilePath(contentPath), openFlags);
	}

	bool UnpackedEngineFileSystem::FileExists(const FilePath& contentPath) const
	{
		return File::Exists(GetFullFilePath(contentPath));
	}

	File UnpackedEngineFileSystem::CreateFile(const FilePath& contentPath, FileOpenFlags openFlags)
	{
		FilePath fullPath = GetFullFilePath(contentPath);
		FilePath directory = fullPath.GetParentDirectory();

		std::filesystem::create_directories(directory.ToString());

		return File(fullPath, openFlags);
	}

	FilePath UnpackedEngineFileSystem::GetFullFilePath(const FilePath& contentPath) const
	{
		if (contentPath.IsRelative())
			return _contentBasePath / contentPath;
		else
			return contentPath;
	}
}