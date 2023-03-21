#pragma once

#include <Coco/Core/Core.h>

#include "Resource.h"
#include "ResourceTypes.h"

namespace Coco
{
	/// <summary>
	/// Base class that provides file save and load operations for a resource
	/// </summary>
	class COCOAPI ResourceLoader
	{
	public:
		/// <summary>
		/// The base path for all resources
		/// </summary>
		const string BasePath;

	protected:
		ResourceLoader(const string& basePath);

	public:
		virtual ~ResourceLoader() = default;

		/// <summary>
		/// Gets the type of resources that this loader can work with
		/// </summary>
		/// <returns>The type of resources that this loader can work with</returns>
		virtual const char* GetResourceTypename() const noexcept = 0;

		/// <summary>
		/// Loads a resource from a file
		/// </summary>
		/// <param name="path">The file path</param>
		/// <returns>The loaded resource</returns>
		Ref<Resource> Load(const string& path);

		/// <summary>
		/// Saves a resource to a file
		/// </summary>
		/// <param name="resource">The resource</param>
		/// <param name="path">The path of the saved file</param>
		void Save(const Ref<Resource>& resource, const string& path);

	protected:
		/// <summary>
		/// Implementation for loading a resource from a file
		/// </summary>
		/// <param name="path">The file path</param>
		/// <returns>The loaded resource</returns>
		virtual Ref<Resource> LoadImpl(const string& path) = 0;

		/// <summary>
		/// Implementation for saving a resource to a file
		/// </summary>
		/// <param name="resource">The resource</param>
		/// <param name="path">The path of the saved file</param>
		virtual void SaveImpl(const Ref<Resource>& resource, const string& path) = 0;
	};
}