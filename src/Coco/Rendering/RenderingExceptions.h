#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// @brief A generic rendering exception
	class COCOAPI RenderingException : public Exception
	{
	public:
		RenderingException(const string& message);
		virtual ~RenderingException() = default;
	};

	/// @brief An exception thrown when a rendering operation is invalid for an object's current state
	class COCOAPI RenderingOperationException : public RenderingException
	{
	public:
		RenderingOperationException(const string& message);
		virtual ~RenderingOperationException() = default;
	};

	/// @brief An exception thrown when a graphics platform fails to initialize
	class COCOAPI GraphicsPlatformInitializeException : public RenderingException
	{
	public:
		GraphicsPlatformInitializeException(const string& message);
		virtual ~GraphicsPlatformInitializeException() = default;
	};

	/// @brief An exception thrown when a buffer transfer fails
	class COCOAPI BufferTransferException : public RenderingException
	{
	public:
		BufferTransferException(const string& message);
		virtual ~BufferTransferException() = default;
	};

	/// @brief An exception thrown when a rendering surface cannot be initialized
	class COCOAPI SurfaceInitializationException : public RenderingException
	{
	public:
		SurfaceInitializationException(const string& message);
		virtual ~SurfaceInitializationException() = default;
	};
}