#pragma once

#include <Coco/Core/Core.h>

#include <atomic>

namespace Coco
{
	using ResourceID = uint64_t;

	class COCOAPI Resource
	{
	private:
		static std::atomic<ResourceID> s_ResourceIndex;
		ResourceID _id = InvalidID;

	public:
		const static ResourceID InvalidID = std::numeric_limits<ResourceID>::max();

	public:
		Resource() noexcept;
		virtual ~Resource() noexcept = default;

		ResourceID GetID() const noexcept { return _id; }

	private:
		static ResourceID GetNewID() noexcept;
	};
}