#include "Corepch.h"
#include "FilePath.h"

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
	{
		_filePath.make_preferred();
	}

	bool FilePath::operator==(const FilePath& other) const
	{
		return _filePath == other._filePath;
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

	FilePath FilePath::ChangeExtension(const string& newExtension) const
	{
		string pathStr = ToString();

		size_t extensionPos = pathStr.find_last_of('.');

		if (extensionPos != string::npos)
			pathStr = pathStr.substr(0, extensionPos);

		return FormatString("{}.{}", pathStr, newExtension);
	}

	bool FilePath::IsRelative() const
	{
		return _filePath.is_relative();
	}

	bool FilePath::IsDirectory() const
	{
		// HACK: for some reason, this is false when checking parent paths? Current workaround is anything with an extension is a file
		//return std::filesystem::is_directory(_filePath);

		return _filePath.has_stem() && !_filePath.has_extension();
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