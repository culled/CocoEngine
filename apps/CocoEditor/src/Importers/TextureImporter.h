#pragma once
#include "ResourceImporter.h"

namespace Coco
{
    class TextureImporter :
        public ResourceImporter
    {
    public:
        // Inherited via ResourceImporter
        bool SupportsExtension(const string& extension) override;
        SharedRef<Resource> Import(const FilePath& path, std::optional<FilePath> saveDirectory) override;
    };
}