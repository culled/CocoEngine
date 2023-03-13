#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	class COCOAPI IResource
	{
	public:
		const static uint64_t InvalidID = std::numeric_limits<uint64_t>::max();

	public:
		virtual ~IResource() = default;

		virtual uint64_t GetID() const = 0;
	};
}