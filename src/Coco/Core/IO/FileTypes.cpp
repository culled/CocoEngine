#include "Corepch.h"
#include "FileTypes.h"

namespace Coco
{
	FilePath::FilePath() :
		_filePath()
	{}

	FilePath::FilePath(const string& filePath) :
		FilePath(std::filesystem::path(filePath))
	{}

	FilePath::FilePath(const std::filesystem::path& filePath) :
		_filePath(filePath)
	{}

	bool FilePath::IsRelativePath(const string& path)
	{
		FilePath p(path);
		return p.IsRelative();
	}

	FilePath FilePath::GetCurrentWorkingDirectoryPath()
	{
		return FilePath(std::filesystem::current_path());
	}

	string FilePath::GetFileName(bool includeExtension) const
	{
		if (includeExtension)
		{
			return _filePath.filename().string();
		}
		else
		{
			return _filePath.stem().string();
		}
	}

	string FilePath::GetExtension() const
	{
		return _filePath.extension().string();
	}

	FilePath FilePath::GetParentDirectory() const
	{
		return FilePath(_filePath.parent_path());
	}

	bool FilePath::IsRelative() const
	{
		return _filePath.is_relative();
	}

	bool FilePath::IsDirectory() const
	{
		return std::filesystem::is_directory(_filePath);
	}

	string FilePath::ToString() const
	{
		return _filePath.string();
	}
}