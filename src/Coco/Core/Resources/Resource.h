#pragma once

#include <Coco/Core/Core.h>

#include "ResourceTypes.h"
#include <atomic>

namespace Coco
{
	/// <summary>
	/// A resource ID
	/// </summary>
	using ResourceID = uint64_t;

	/// <summary>
	/// A resource version number
	/// </summary>
	using ResourceVersion = uint64_t;

	/// <summary>
	/// A generic cached resource
	/// </summary>
	struct CachedResource
	{
		ResourceID ID;
		ResourceVersion Version;
		uint64_t LastTickUsed;

		CachedResource(ResourceID id, ResourceVersion version);
		virtual ~CachedResource() = default;

		virtual bool IsInvalid() const noexcept = 0;
		virtual bool NeedsUpdate() const noexcept = 0;

		void UpdateTickUsed();
		bool ShouldPurge(uint64_t staleTickThreshold) const noexcept;
	};

	/// <summary>
	/// A generic resource for the engine
	/// </summary>
	class COCOAPI Resource
	{
		friend class ResourceLoader;

	private:
		static std::atomic<ResourceID> s_ResourceIndex;
		ResourceID _id = InvalidID;
		std::atomic<ResourceVersion> _version = 0;
		string _typename;
		string _filePath;
		string _name;

	public:
		/// <summary>
		/// An invalid ID
		/// </summary>
		constexpr static ResourceID InvalidID = Math::MaxValue<ResourceID>();

	protected:
		Resource(const string& name, const string& namedType) noexcept;
		Resource(const string& name, ResourceType type) noexcept;

	public:
		virtual ~Resource() noexcept = default;

		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;
		Resource(Resource&&) = delete;
		Resource& operator=(Resource&&) = delete;

		/// <summary>
		/// Gets this resource's ID
		/// </summary>
		/// <returns>This resource's unique ID</returns>
		ResourceID GetID() const noexcept { return _id; }

		/// <summary>
		/// Gets this resource's version number
		/// </summary>
		/// <returns>This resource's version number</returns>
		ResourceVersion GetVersion() const noexcept { return _version; }

		/// <summary>
		/// Gets the type of this resource
		/// </summary>
		/// <returns>This resource's type</returns>
		const string& GetTypename() const noexcept { return _typename; }

		/// <summary>
		/// Gets the path to the file this resource is loaded from (if any)
		/// </summary>
		/// <returns>The path to the file this resource is loaded from, or an empty string if this resource isn't saved to disk</returns>
		const string& GetFilePath() const noexcept { return _filePath; }

		/// <summary>
		/// Gets the name of this resource
		/// </summary>
		/// <returns>This resource's name</returns>
		const string& GetName() const noexcept { return _name; }

	protected:
		/// <summary>
		/// Increments this resource's version number
		/// </summary>
		void IncrementVersion() noexcept { _version++; }

		/// <summary>
		/// Sets this resource's version number
		/// </summary>
		/// <param name="version">The new version number</param>
		void SetVersion(ResourceVersion version) noexcept { _version.store(version); }

	private:
		/// <summary>
		/// Gets a new unique ID
		/// </summary>
		/// <returns>A unique ID</returns>
		static ResourceID GetNewID() noexcept;

		/// <summary>
		/// Sets this resource's file path
		/// </summary>
		/// <param name="filePath">The file path</param>
		void SetFilePath(const string& filePath) { _filePath = filePath; }
	};
}