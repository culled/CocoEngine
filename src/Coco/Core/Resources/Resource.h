#pragma once

#include <Coco/Core/Core.h>

#include "ResourceTypes.h"
#include <atomic>

#define DeclareResourceType std::type_index GetType() const noexcept override;
#define DefineResourceType(Type) std::type_index GetType() const noexcept override { return typeid(Type); }

namespace Coco
{
	/// @brief A generic resource for the engine
	class COCOAPI Resource
	{
	public:
		/// @brief An invalid ID
		constexpr static ResourceID InvalidID = Math::MaxValue<ResourceID>();
		constexpr static uint64_t MaxLifetime = Math::MaxValue<uint64_t>();

	protected:
		ResourceID _id;
		ResourceVersion _version = 0;
		string _name;
		uint64_t _lastTickUsed;
		uint64_t _tickLifetime;

	protected:
		Resource(ResourceID id, const string& name, uint64_t tickLifetime) noexcept;

	public:
		virtual ~Resource() noexcept = default;

		ResourceID GetID() const noexcept { return _id; }

		/// @brief Gets this resource's version number
		/// @return This resource's version number
		ResourceVersion GetVersion() const noexcept { return _version; }

		/// @brief Gets this resource's name
		/// @return This resource's name
		const string& GetName() const noexcept { return _name; }

		void SetName(const string& name) noexcept { _name = name; }

		/// @brief Gets the type that this resource is
		/// @return This resource's type
		virtual std::type_index GetType() const noexcept = 0;

		/// @brief Gets the last tick that this cached resource was used
		/// @return The last this that this cached resource was used
		constexpr uint64_t GetLastTickUsed() const noexcept { return _lastTickUsed; }

		/// @brief Gets the lifetime of this cached resource
		/// @return The lifetime of this cached resource
		constexpr uint64_t GetTickLifetime() const noexcept { return _tickLifetime; }

		/// @brief Updates the last tick that this cached resource was used to the current tick
		void UpdateTickUsed();

		/// @brief Determines if this cached resource is stale (hasn't been used beyond its lifetime)
		/// @return True if this cached resource has been unused longer than its lifetime
		bool IsStale() const noexcept;

	protected:
		/// @brief Increments this resource's version number
		void IncrementVersion() noexcept { _version++; }

		/// @brief Sets this resource's version number
		/// @param version The new version number
		void SetVersion(ResourceVersion version) noexcept { _version = version; }
	};
}