#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Platform
{
	/// <summary>
	/// A generic exception thrown by an engine platform
	/// </summary>
	class COCOAPI EnginePlatformException : public Exception
	{
	public:
		EnginePlatformException(const string& message);
		virtual ~EnginePlatformException() = default;
	};

	/// <summary>
	/// Thrown when an engine platform fails to initialize
	/// </summary>
	class COCOAPI EnginePlatformInitializeException : public EnginePlatformException
	{
	public:
		EnginePlatformInitializeException(const string& message);
		virtual ~EnginePlatformInitializeException() = default;
	};

	/// <summary>
	/// Thrown when a platform operation fails
	/// </summary>
	class COCOAPI EnginePlatformOperationException : public EnginePlatformException
	{
	public:
		EnginePlatformOperationException(const string& message);
		virtual ~EnginePlatformOperationException() = default;
	};
}