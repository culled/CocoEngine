#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Windowing
{
	/// <summary>
	/// A generic window exception
	/// </summary>
	class COCOAPI WindowException : public Exception
	{
	public:
		WindowException(const string& message);
		virtual ~WindowException() = default;
	};

	/// <summary>
	/// An exception thrown when a window fails to create
	/// </summary>
	class COCOAPI WindowCreateException : public WindowException
	{
	public:
		WindowCreateException(const string& message);
		virtual ~WindowCreateException() = default;
	};

	/// <summary>
	/// An exception thrown when a window operation fails
	/// </summary>
	class COCOAPI WindowOperationException : public WindowException
	{
	public:
		WindowOperationException(const string& message);
		virtual ~WindowOperationException() = default;
	};
}