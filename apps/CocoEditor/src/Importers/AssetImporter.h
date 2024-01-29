#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/IO/FilePath.h>

namespace Coco
{
	class AssetImporter
	{
	public:
		virtual ~AssetImporter() = default;

		virtual SharedRef<Resource> Import(const FilePath& filePath) = 0;
	};
}