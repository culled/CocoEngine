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

	File UnpackedEngineFileSystem::OpenFile(const string& contentPath, FileOpenFlags openFlags)
	{
		return File(GetFullFilePath(contentPath), openFlags);
	}

	bool UnpackedEngineFileSystem::FileExists(const string& contentPath) const
	{
		return File::Exists(GetFullFilePath(contentPath));
	}

	File UnpackedEngineFileSystem::CreateFile(const string& contentPath, FileOpenFlags openFlags)
	{
		FilePath fullPath = GetFullFilePath(contentPath);
		FilePath directory = fullPath.GetParentDirectory();

		std::filesystem::create_directories(directory.ToString());

		return File(fullPath, openFlags);
	}

	FilePath UnpackedEngineFileSystem::GetFullFilePath(const string& contentPath) const
	{
		if (FilePath::IsRelativePath(contentPath))
			return FilePath::CombineToPath(_contentBasePath, contentPath);
		else
			return FilePath(contentPath);
	}
}