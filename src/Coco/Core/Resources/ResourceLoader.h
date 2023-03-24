#pragma once

#include <Coco/Core/Core.h>

#include "Resource.h"
#include "ResourceTypes.h"

namespace Coco
{
	class ResourceLibrary;

	/// @brief Provides file save and load operations for a resource
	class COCOAPI ResourceLoader
	{
	public:
		/// @brief The resource library
		ResourceLibrary* const Library;

	protected:
		ResourceLoader(ResourceLibrary* library);

	public:
		virtual ~ResourceLoader() = default;

		/// @brief Gets the type of resources that this loader can work with
		/// @return The type of resources that this loader can work with
		virtual string GetResourceTypename() const noexcept = 0;

		/// @brief Loads a resource from a file
		/// @param path The file path
		/// @return The loaded resource
		Ref<Resource> Load(const string& path);

		/// @brief Saves a resource to a file
		/// @param resource The resource
		/// @param path The path of the saved file
		void Save(const Ref<Resource>& resource, const string& path);

	protected:
		/// @brief Implementation for loading a resource from a file
		/// @param path The file path
		/// @return The loaded resource
		virtual Ref<Resource> LoadImpl(const string& path) = 0;

		/// @brief Implementation for saving a resource to a file
		/// @param resource The resource
		/// @param path The path of the saved file
		virtual void SaveImpl(const Ref<Resource>& resource, const string& path) = 0;
	};
}