#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// @brief A generic file exception
	class COCOAPI FileException : public Exception
	{
	public:
		FileException(const string& message);
		virtual ~FileException() = default;
	};

	/// @brief An exception when a file failes to open
	class COCOAPI FileOpenException : public FileException
	{
	public:
		FileOpenException(const string& message);
		virtual ~FileOpenException() = default;
	};
}