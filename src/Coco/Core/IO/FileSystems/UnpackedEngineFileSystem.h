#pragma once
#include "../EngineFileSystem.h"

namespace Coco
{
    /// @brief An EngineFileSystem that uses unpacked files in a content directory
    class UnpackedEngineFileSystem :
        public EngineFileSystem
    {
    private:
        FilePath _contentBasePath;

    public:
        UnpackedEngineFileSystem(const FilePath& contentBasePath);

        File OpenFile(const FilePath& contentPath, FileOpenFlags openFlags) final;
        bool FileExists(const FilePath& contentPath) const final;
        FilePath ConvertToFullPath(const FilePath& contentPath) const final;
        File CreateFile(const FilePath& contentPath, FileOpenFlags openFlags) final;

        /// @brief Gets the path to the content directory
        /// @return The content directory path
        const FilePath& GetContentBasePath() const { return _contentBasePath; }
    };
}