#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/String.h>
#include <stdexcept>

namespace Coco
{
	/// <summary>
	/// A generic exception
	/// </summary>
	class COCOAPI Exception : public std::exception
	{
	public:
		Exception() = default;
		Exception(const string& message);
		virtual ~Exception() = default;
	};

	/// <summary>
	/// An exception thrown when an index is out of an expected range
	/// </summary>
	using IndexOutOfRangeException = Exception;

	/// <summary>
	/// A generic exception thrown when an object fails to create
	/// </summary>
	using ObjectCreateException = Exception;
}