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
	class COCOAPI IndexOutOfRangeException : public Exception
	{
	public:
		IndexOutOfRangeException(const string& message);
		virtual ~IndexOutOfRangeException() = default;
	};

	/// <summary>
	/// An exception thrown when an operation is invalid for an object's current state
	/// </summary>
	class COCOAPI InvalidOperationException : public Exception
	{
	public:
		InvalidOperationException(const string& message);
		virtual ~InvalidOperationException() = default;
	};
}