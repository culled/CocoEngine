#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Platform
{
	/// @brief A generic exception thrown by an engine platform
	class COCOAPI EnginePlatformException : public Exception
	{
	public:
		EnginePlatformException(const string& message);
		virtual ~EnginePlatformException() = default;
	};

	/// @brief Thrown when an engine platform fails to initialize
	class COCOAPI EnginePlatformInitializeException : public EnginePlatformException
	{
	public:
		EnginePlatformInitializeException(const string& message);
		virtual ~EnginePlatformInitializeException() = default;
	};

	/// @brief Thrown when a platform operation fails
	class COCOAPI EnginePlatformOperationException : public EnginePlatformException
	{
	public:
		EnginePlatformOperationException(const string& message);
		virtual ~EnginePlatformOperationException() = default;
	};
}