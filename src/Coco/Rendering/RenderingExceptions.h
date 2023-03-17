#pragma once

#include <Coco/Core/Core.h>

namespace Coco::Rendering
{
	/// <summary>
	/// A generic rendering exception
	/// </summary>
	class COCOAPI RenderingException : public Exception
	{
	public:
		RenderingException(const string& message);
		virtual ~RenderingException() = default;
	};

	/// <summary>
	/// An exception thrown when a rendering operation is invalid for an object's current state
	/// </summary>
	class COCOAPI RenderingOperationException : public RenderingException
	{
	public:
		RenderingOperationException(const string& message);
		virtual ~RenderingOperationException() = default;
	};

	/// <summary>
	/// An exception thrown when a graphics platform fails to initialize
	/// </summary>
	class COCOAPI GraphicsPlatformInitializeException : public RenderingException
	{
	public:
		GraphicsPlatformInitializeException(const string& message);
		virtual ~GraphicsPlatformInitializeException() = default;
	};

	/// <summary>
	/// An exception thrown when a buffer transfer fails
	/// </summary>
	class COCOAPI BufferTransferException : public RenderingException
	{
	public:
		BufferTransferException(const string& message);
		virtual ~BufferTransferException() = default;
	};

	/// <summary>
	/// An exception thrown when a rendering surface cannot be initialized
	/// </summary>
	class COCOAPI SurfaceInitializationException : public RenderingException
	{
	public:
		SurfaceInitializationException(const string& message);
		virtual ~SurfaceInitializationException() = default;
	};
}