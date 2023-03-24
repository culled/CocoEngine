#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// @brief An exception thrown when an engine service fails to start
	class COCOAPI EngineServiceException : public Exception
	{
	public:
		EngineServiceException(const string& message);
		virtual ~EngineServiceException() = default;
	};

	/// @brief An exception thrown when an engine service fails to start
	class COCOAPI EngineServiceStartException : public EngineServiceException
	{
	public:
		EngineServiceStartException(const string& message);
		virtual ~EngineServiceStartException() = default;
	};
}