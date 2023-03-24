#pragma once

#include <Coco/Core/Core.h>

#include "ResourceTypes.h"
#include <atomic>

namespace Coco
{
	/// @brief A resource ID
	using ResourceID = uint64_t;

	/// @brief A resource version number
	using ResourceVersion = uint64_t;

	/// @brief A generic resource for the engine
	class COCOAPI Resource
	{
		friend class ResourceLoader;

	public:
		/// @brief An invalid ID
		constexpr static ResourceID InvalidID = Math::MaxValue<ResourceID>();

		/// @brief The name of this resource
		const string Name;

		/// @brief The type of this resource
		const string TypeName;

		/// @brief The unique ID of this resource
		const ResourceID ID;

	private:
		static std::atomic<ResourceID> s_resourceIndex;
		std::atomic<ResourceVersion> _version = 0;
		string _filePath;

	protected:
		Resource(const string& name, const string& namedType) noexcept;
		Resource(const string& name, ResourceType type) noexcept;

	public:
		virtual ~Resource() noexcept = default;

		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;
		Resource(Resource&&) = delete;
		Resource& operator=(Resource&&) = delete;

		/// @brief Gets this resource's version number
		/// @return This resource's version number
		ResourceVersion GetVersion() const noexcept { return _version; }

		/// @brief Gets the path to the file this resource is loaded from (if any)
		/// @return The path to the file this resource is loaded from, or an empty string if this resource isn't saved to disk
		const string& GetFilePath() const noexcept { return _filePath; }

	protected:
		/// @brief Increments this resource's version number
		void IncrementVersion() noexcept { _version++; }

		/// @brief Sets this resource's version number
		/// @param version The new version number
		void SetVersion(ResourceVersion version) noexcept { _version.store(version); }

	private:
		/// @brief Gets a new unique ID
		/// @return A unique ID
		static ResourceID GetNewID() noexcept;

		/// @brief Sets this resource's file path
		/// @param filePath The file path
		void SetFilePath(const string& filePath) { _filePath = filePath; }
	};

	/// @brief A generic cached resource
	class COCOAPI CachedResource
	{
	public:
		/// @brief The ID of the resource that this cached resource references
		const ResourceID ID;

	private:
		ResourceVersion _version;
		uint64_t _lastTickUsed;

	public:
		CachedResource(ResourceID id, ResourceVersion version);
		virtual ~CachedResource() = default;

		/// @brief Gets the version of this cached resource
		/// @return The version of this cached resource
		constexpr ResourceVersion GetVersion() const noexcept { return _version; }

		/// @brief Gets the last tick that this cached resource was used
		/// @return The last this that this cached resource was used
		constexpr uint64_t GetLastTickUsed() const noexcept { return _lastTickUsed; }

		/// @brief Determines if this cached resource is invalid and can't be used anymore
		/// @return True if this cached resource is invalid
		virtual bool IsInvalid() const noexcept = 0;

		/// @brief Determines if this cached resource needs to be updated
		/// @return True if this cached resource should be updated
		virtual bool NeedsUpdate() const noexcept = 0;

		/// @brief Updates the last tick that this cached resource was used to the current tick
		void UpdateTickUsed();

		/// @brief Updates this cached resource's version
		/// @param newVersion The new version
		constexpr void UpdateVersion(ResourceVersion newVersion) noexcept { _version = newVersion; }

		/// @brief Determines if this cached resource should be purged
		/// @param staleTickThreshold The threshold for determining a stale cached resource
		/// @return True if this cached resource should be purged
		bool ShouldPurge(uint64_t staleTickThreshold) const noexcept;
	};
}