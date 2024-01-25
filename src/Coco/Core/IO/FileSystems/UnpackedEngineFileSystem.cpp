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

	File UnpackedEngineFileSystem::OpenFile(const FilePath& contentPath, FileOpenFlags openFlags, bool createIfNonexistent)
	{
		FilePath path = ConvertToFullPath(contentPath);

		if (!FileExists(path) && createIfNonexistent)
			return CreateFile(path, openFlags);

		return File(ConvertToFullPath(path), openFlags);
	}

	bool UnpackedEngineFileSystem::FileExists(const FilePath& contentPath) const
	{
		return File::Exists(ConvertToFullPath(contentPath));
	}

	FilePath UnpackedEngineFileSystem::ConvertToFullPath(const FilePath& contentPath) const
	{
		if (contentPath.IsRelative())
			return _contentBasePath / contentPath;
		else
			return contentPath;
	}

	FilePath UnpackedEngineFileSystem::ConvertToShortPath(const FilePath& contentPath) const
	{
		if (contentPath.IsRelative())
			return contentPath;
		else
			return FilePath::GetRelativePath(contentPath, _contentBasePath);
	}

	bool UnpackedEngineFileSystem::AreSameFile(const FilePath& filePath1, const FilePath& filePath2) const
	{
		if (filePath1.IsEmpty() || filePath2.IsEmpty())
			return filePath1 == filePath2;

		FilePath fullPath1 = ConvertToFullPath(filePath1);
		FilePath fullPath2 = ConvertToFullPath(filePath2);
		return std::filesystem::equivalent(fullPath1, fullPath2);
	}

	File UnpackedEngineFileSystem::CreateFile(const FilePath& contentPath, FileOpenFlags openFlags)
	{
		FilePath fullPath = ConvertToFullPath(contentPath);
		FilePath directory = fullPath.GetParentDirectory();

		std::filesystem::create_directories(directory.ToString());

		return File(fullPath, openFlags);
	}
}