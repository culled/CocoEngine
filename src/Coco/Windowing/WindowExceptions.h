#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Windowing
{
	/// @brief A generic window exception
	class COCOAPI WindowException : public Exception
	{
	public:
		WindowException(const string& message);
		virtual ~WindowException() = default;
	};

	/// @brief An exception thrown when a window fails to create
	class COCOAPI WindowCreateException : public WindowException
	{
	public:
		WindowCreateException(const string& message);
		virtual ~WindowCreateException() = default;
	};

	/// @brief An exception thrown when a window operation fails
	class COCOAPI WindowOperationException : public WindowException
	{
	public:
		WindowOperationException(const string& message);
		virtual ~WindowOperationException() = default;
	};
}