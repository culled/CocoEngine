#pragma once

#include <Coco/Core/Core.h>

#include <atomic>

namespace Coco
{
	using ResourceID = uint64_t;

	/// <summary>
	/// A generic resource for the engine
	/// </summary>
	class COCOAPI Resource
	{
	private:
		static std::atomic<ResourceID> s_ResourceIndex;
		ResourceID _id = InvalidID;

	public:
		/// <summary>
		/// An invalid ID
		/// </summary>
		const static ResourceID InvalidID = std::numeric_limits<ResourceID>::max();

	public:
		Resource() noexcept;
		virtual ~Resource() noexcept = default;

		Resource(const Resource&) = delete;
		Resource(Resource&&) = delete;

		Resource& operator=(const Resource&) = delete;
		Resource& operator=(Resource&&) = delete;

		/// <summary>
		/// Gets this resource's ID
		/// </summary>
		/// <returns>This resource's unique ID</returns>
		ResourceID GetID() const noexcept { return _id; }

	private:
		/// <summary>
		/// Gets a new unique ID
		/// </summary>
		/// <returns>A unique ID</returns>
		static ResourceID GetNewID() noexcept;
	};
}