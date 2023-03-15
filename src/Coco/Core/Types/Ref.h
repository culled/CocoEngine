#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <memory>

namespace Coco
{
	/// <summary>
	/// Class that manages the lifetime of an object and can be shared
	/// </summary>
	/// <typeparam name="T">The type of object</typeparam>
	template<class T>
	using Ref = std::shared_ptr<T>;
	
	/// <summary>
	/// Creates a managed object that can be shared
	/// </summary>
	/// <typeparam name="T">The type of object</typeparam>
	/// <typeparam name="...Args">Arguments to pass to the object constructor</typeparam>
	/// <param name="...args">Arguments to pass to the object constructor</param>
	/// <returns>A managed object that can be shared</returns>
	template<class T, typename ... Args>
	Ref<T> COCOAPI CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	/// <summary>
	/// A weak reference to a reference
	/// </summary>
	/// <typeparam name="T">The type of object</typeparam>
	template<class T>
	using WeakRef = std::weak_ptr<T>;
	
	/// <summary>
	/// Class that manages the lifetime of an object
	/// </summary>
	/// <typeparam name="T">The type of managed object</typeparam>
	/// <typeparam name="U">An optional deleter for the managed object</typeparam>
	template<class T, typename U = std::default_delete<T>>
	using Managed = std::unique_ptr<T, U>;

	/// <summary>
	/// Creates a managed object that can be shared
	/// </summary>
	/// <typeparam name="T">The type of object</typeparam>
	/// <typeparam name="...Args">Arguments to pass to the object constructor</typeparam>
	/// <param name="...args">Arguments to pass to the object constructor</param>
	/// <returns>A managed object</returns>
	template<class T, typename ... Args>
	Managed<T> COCOAPI CreateManaged(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	/// <summary>
	/// Creates a managed object that can be shared
	/// </summary>
	/// <typeparam name="T">The type of object</typeparam>
	/// <typeparam name="...Args">Arguments to pass to the object constructor</typeparam>
	/// <param name="...args">Arguments to pass to the object constructor</param>
	/// <returns>A managed object</returns>
	template<class T, typename U, typename ... Args>
	Managed<T, U> COCOAPI CreateManaged(Args&& ... args)
	{
		return std::make_unique<T, U>(std::forward<Args>(args)...);
	}
}