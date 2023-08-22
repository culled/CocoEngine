#pragma once

#include <Coco/Core/Core.h>

#include "ResourceTypes.h"
#include <Coco/Core/Events/Event.h>
#include <atomic>

#define DeclareResourceType std::type_index GetType() const noexcept override;
#define DefineResourceType(Type) std::type_index GetType() const noexcept override { return typeid(Type); }

namespace Coco
{
	/// @brief A resource that can be held in a ResourceLibrary
	class COCOAPI Resource
	{
	public:
		/// @brief An invalid ID
		const static ResourceID InvalidID;

		/// @brief The ID of this resource
		const ResourceID ID;

	protected:
		ResourceVersion _version = 0;
		string _name;
		uint64_t _lastTickUsed;
		string _filePath;

	protected:
		Resource(const ResourceID& id, const string& name) noexcept;

	public:
		virtual ~Resource() noexcept = default;

		/// @brief Gets this resource's version
		/// @return This resource's version
		ResourceVersion GetVersion() const noexcept { return _version; }

		/// @brief Gets this resource's name
		/// @return This resource's name
		const string& GetName() const noexcept { return _name; }

		/// @brief Sets this resource's name
		/// @param name The name
		void SetName(const string& name) noexcept { _name = name; }

		/// @brief Gets this resource's type
		/// @return This resource's type
		virtual std::type_index GetType() const noexcept = 0;

		/// @brief Sets this resource's file path
		/// @param filePath The file path
		void SetFilePath(const string& filePath) { _filePath = filePath; }

		/// @brief Gets the path to the file that this resource is loaded from (if any)
		/// @return The path to the file that this resource is loaded from, or an empty string if this resource isn't saved to disk
		const string& GetFilePath() const noexcept { return _filePath; }

		/// @brief Gets if this resource has a file path. This usually means it relates to a file on the disk
		/// @return True if this resource has a file path
		bool HasFilePath() const noexcept { return !_filePath.empty(); }

		/// @brief Gets the last tick that this cached resource was used
		/// @return The last this that this cached resource was used
		constexpr uint64_t GetLastTickUsed() const noexcept { return _lastTickUsed; }

		/// @brief Updates the last tick that this resource was used to the current tick
		void UpdateTickUsed();

	protected:
		/// @brief Increments this resource's version number
		void IncrementVersion() noexcept { _version++; }

		/// @brief Sets this resource's version number
		/// @param version The new version number
		void SetVersion(ResourceVersion version) noexcept { _version = version; }
	};
}