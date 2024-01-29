#pragma once
#include "../AssetImporter.h"

namespace Coco
{
    class TextureImporter :
        public AssetImporter
    {
        // Inherited via AssetImporter
        SharedRef<Resource> Import(const FilePath& filePath) override;
    };
}