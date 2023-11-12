#pragma once

#include <optional>
#include <Coco/Core/IO/FileTypes.h>
#include <Coco/Core/Resources/Resource.h>

namespace Coco
{
	class ResourceImporter
	{
	public:
		virtual ~ResourceImporter() = default;

		virtual bool SupportsExtension(const string& extension) = 0;
		virtual SharedRef<Resource> Import(const FilePath& path, std::optional<FilePath> saveDirectory) = 0;
	};
}