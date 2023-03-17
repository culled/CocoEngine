#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// A generic file exception
	/// </summary>
	class COCOAPI FileException : public Exception
	{
	public:
		FileException(const string& message);
		virtual ~FileException() = default;
	};

	/// <summary>
	/// An exception when a file failes to open
	/// </summary>
	class COCOAPI FileOpenException : public FileException
	{
	public:
		FileOpenException(const string& message);
		virtual ~FileOpenException() = default;
	};
}