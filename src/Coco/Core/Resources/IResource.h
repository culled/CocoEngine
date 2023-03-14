#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	using ResourceID = uint64_t;

	class COCOAPI IResource
	{
	public:
		const static ResourceID InvalidID = std::numeric_limits<ResourceID>::max();

	public:
		virtual ~IResource() = default;

		virtual ResourceID GetID() const = 0;
	};
}