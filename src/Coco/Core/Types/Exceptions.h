#pragma once

#include <Coco/Core/API.h>
#include <Coco/Core/Types/String.h>
#include <stdexcept>

namespace Coco
{
	/// @brief A generic exception
	class COCOAPI Exception : public std::exception
	{
	public:
		Exception() = default;
		Exception(const string& message) noexcept;
		virtual ~Exception() = default;
	};

	/// @brief An exception thrown when an index is out of an expected range
	class COCOAPI IndexOutOfRangeException : public Exception
	{
	public:
		IndexOutOfRangeException(const string& message) noexcept;
		virtual ~IndexOutOfRangeException() = default;
	};

	/// @brief An exception thrown when an operation is invalid for an object's current state
	class COCOAPI InvalidOperationException : public Exception
	{
	public:
		InvalidOperationException(const string& message) noexcept;
		virtual ~InvalidOperationException() = default;
	};
}