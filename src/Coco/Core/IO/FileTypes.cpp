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

	FilePath FilePath::GetCurrentWorkingDirectoryPath()
	{
		return FilePath(std::filesystem::current_path());
	}

	string FilePath::ToString() const
	{
		return _filePath.string();
	}
}