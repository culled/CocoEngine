#include "EngineServiceExceptions.h"

namespace Coco
{
	EngineServiceException::EngineServiceException(const string& message) : Exception(message)
	{
	}

	EngineServiceStartException::EngineServiceStartException(const string& message) : EngineServiceException(message)
	{
	}
}
