#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// An exception thrown when an engine service fails to start
	/// </summary>
	class COCOAPI EngineServiceException : public Exception
	{
	public:
		EngineServiceException(const string& message);
		virtual ~EngineServiceException() = default;
	};

	/// <summary>
	/// An exception thrown when an engine service fails to start
	/// </summary>
	class COCOAPI EngineServiceStartException : public EngineServiceException
	{
	public:
		EngineServiceStartException(const string& message);
		virtual ~EngineServiceStartException() = default;
	};
}