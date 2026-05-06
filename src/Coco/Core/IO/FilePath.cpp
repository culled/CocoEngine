//
// Created by cullen on 3/10/26.
//

#include "FilePath.h"

#include <filesystem>
#include <utility>

namespace Coco
{
    FilePath operator/(const FilePath& a, const FilePath& b)
    {
        FilePath v(a);
        v /= b;
        return v;
    }

    FilePath operator/(const FilePath& a, const String& b)
    {
        FilePath v(a);
        v /= b;
        return v;
    }

    FilePath operator/(const FilePath& a, const char* b)
    {
        FilePath v(a);
        v /= b;
        return v;
    }

    FilePath::FilePath() :
        _path()
    {}

    FilePath::FilePath(const char* path) :
        _path(path)
    {}

    FilePath::FilePath(String path) :
        _path(std::move(path))
    {}

    FilePath FilePath::GetCurrentWorkingDirectory()
    {
        return {std::filesystem::current_path().c_str()};
    }

    void FilePath::CreateDirectories(const FilePath& path)
    {
        std::filesystem::create_directories(path._path.CStr());
    }

    FilePath FilePath::GetRelativePath(const FilePath& base, const FilePath& path)
    {
        return {std::filesystem::relative(path._path.CStr(), base._path.CStr()).c_str()};
    }

    void FilePath::operator/=(const FilePath& rhs)
    {
        _path.Reserve(_path.GetLength() + rhs._path.GetLength() + 1);
        _path.Append("/");
        _path.Append(rhs._path);
    }

    FilePath FilePath::GetParentDirectory() const
    {
        uint64 searchOffset = _path.EndsWith('/') ? 1 : 0;
        int64 endOffset = _path.LastIndexOf('/', searchOffset);
        return {_path.GetSubstring(0, endOffset)};
    }

    String FilePath::GetFileName(bool includeExtension) const
    {
        int64 dirOffset = _path.LastIndexOf('/');
        String endPart = _path.GetSubstring(dirOffset + 1);

        int64 extOffset = endPart.LastIndexOf('.');
        if (!includeExtension && extOffset != -1)
        {
            endPart.Resize(extOffset);
        }

        return endPart;
    }

    String FilePath::GetExtension() const
    {
        int64 extOffset = _path.LastIndexOf('.');

        if (extOffset == -1)
            return {};

        return _path.GetSubstring(extOffset);
    }

    FilePath FilePath::WithExtension(const char* newExtension) const
    {
        int64 extOffset = _path.LastIndexOf('.');
        String newPath = _path.GetSubstring(0, extOffset);
        newPath.Append(newExtension);

        return {newPath};
    }

    bool FilePath::IsRelative() const noexcept
    {
        return !(_path.StartsWith('/') || _path.Find(':') != -1);
    }

    bool FilePath::IsDirectory() const noexcept
    {
        int64 dirOffset = _path.LastIndexOf('/');
        int64 extOffset = _path.LastIndexOf('.');

        return dirOffset > extOffset;
    }

    void swap(FilePath& a, FilePath& b) noexcept
    {
        using std::swap;
        swap(a._path, b._path);
    }
} // Coco