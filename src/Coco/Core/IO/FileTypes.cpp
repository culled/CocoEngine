#include "Corepch.h"
#include "FileTypes.h"

namespace Coco
{
	FilePath::FilePath() :
		_filePath()
	{}

	FilePath::FilePath(const char* filePath) :
		FilePath(std::filesystem::path(filePath))
	{}

	FilePath::FilePath(const string& filePath) :
		FilePath(std::filesystem::path(filePath))
	{}

	FilePath::FilePath(const std::filesystem::path& filePath) :
		_filePath(filePath)
	{}

	bool FilePath::operator==(const FilePath& other) const
	{
		if ((_filePath.empty() || other._filePath.empty()) || (_filePath.is_relative() && other._filePath.is_relative()))
			return _filePath == other._filePath;

		return std::filesystem::equivalent(_filePath, other._filePath);
	}

	FilePath FilePath::operator/(const FilePath& b) const
	{
		return FilePath(this->_filePath / b._filePath);
	}

	void FilePath::operator/=(const FilePath& b)
	{
		_filePath /= b._filePath;
	}

	FilePath FilePath::GetRelativePath(const FilePath& path, const FilePath& relativeTo)
	{
		return FilePath(std::filesystem::relative(path, relativeTo));
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

	bool FilePath::IsEmpty() const
	{
		return _filePath.empty();
	}

	string FilePath::ToString() const
	{
		return _filePath.string();
	}
}