//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_FILEPATH_H
#define COCOENGINE_FILEPATH_H
#include "Coco/Core/Types/String.h"

namespace Coco
{
    class FilePath;
    FilePath operator/(const FilePath& a, const FilePath& b);
    FilePath operator/(const FilePath& a, const String& b);
    FilePath operator/(const FilePath& a, const char* b);

    class FilePath
    {
    public:
        FilePath();
        FilePath(const char* path);
        FilePath(String path);

        /// @brief Combines parts into a path
        /// @tparam ...Args The argument types
        /// @param base The base path fragment
        /// @param first The first path fragment
        /// @param ...others The remaining path fragments
        /// @return The combined file path
        template<typename ... Args>
        static FilePath Combine(const FilePath& base, const FilePath& first, Args&& ... others)
        {
            FilePath combined(base / first);
            return Combine(combined, others...);
        }

        /// @brief Combines parts into a path
        /// @tparam ...Args The argument types
        /// @param base The base path fragment
        /// @param first The first path fragment
        /// @param ...others The remaining path fragments
        /// @return The combined file path
        template<typename ... Args>
        static FilePath Combine(const FilePath& base, const String& first, Args&& ... others)
        {
            FilePath combined(base / first);
            return Combine(combined, others...);
        }

        /// @brief Combines parts into a path
        /// @tparam ...Args The argument types
        /// @param base The base path fragment
        /// @param first The first path fragment
        /// @param ...others The remaining path fragments
        /// @return The combined file path
        template<typename ... Args>
        static FilePath Combine(const FilePath& base, const char* first, Args&& ... others)
        {
            FilePath combined(base / first);
            return Combine(combined, others...);
        }

        template<typename ... Args>
        static FilePath Combine(const char* base, Args&& ... others)
        {
            return Combine(FilePath(base), others...);
        }

        template<typename ... Args>
        static FilePath Combine(const String& base, Args&& ... others)
        {
            return Combine(FilePath(base), others...);
        }

        /// @brief Gets the path of the current working directory
        /// @return The path of the current working directory
        static FilePath GetCurrentWorkingDirectory();

        /// @brief Creates directories for a given path
        /// @param path The path to create directories for
        static void CreateDirectories(const FilePath& path);

        /// @brief Gets a path that is relative to the base path
        /// @param base The base path
        /// @param path The path to convert to a relative path
        /// @return A path that is relative to the base path
        static FilePath GetRelativePath(const FilePath& base, const FilePath& path);

        void operator/=(const FilePath& rhs);

        friend void swap(FilePath& a, FilePath& b) noexcept;

        const char* CStr() const { return _path.CStr(); }
        uint64 GetLength() const { return _path.GetLength(); }

        String AsString() const { return _path; }

        /// @brief Gets the path of the parent directory
        /// @return The parent directory path
        FilePath GetParentDirectory() const;

        /// @brief Gets the name of the file
        /// @param includeExtension If true, the extension will be included in the returned name
        /// @return The name of the file
        String GetFileName(bool includeExtension) const;

        /// @brief Gets the extension (including the ".") of the file name. The extension will always be lowercase
        /// @return The extension
        String GetExtension() const;

        /// @brief Returns this file path with its extension replaced
        /// @param newExtension The new extension without the leading "."
        /// @return The path with the new extension
        FilePath WithExtension(const char* newExtension) const;

        /// @brief Determines if this path is a relative path
        /// @return True if this is a relative path
        bool IsRelative() const noexcept;

        /// @brief Determines if this path points to a directory
        /// @return True if this path is a directory
        bool IsDirectory() const noexcept;

        /// @brief Determines if this path is empty
        /// @return True if this path is empty
        bool IsEmpty() const noexcept { return _path.IsEmpty(); }

    private:
        String _path;

    private:
        /// @brief Resolved for the final template parameter of the Combine function
        /// @param base The path
        /// @return The path
        static FilePath Combine(const FilePath& base)
        {
            return base;
        }
    };
} // Coco

#endif //COCOENGINE_FILEPATH_H