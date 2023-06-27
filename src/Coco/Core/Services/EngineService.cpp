#include "EngineService.h"

#include <Coco/Core/Engine.h>

namespace Coco
{
	EngineService::EngineService(EngineServiceManager* serviceManager) :
		ServiceManager(serviceManager)
	{}

	Logging::Logger* EngineService::GetLogger() const noexcept
	{
		return ServiceManager->Engine->GetLogger();
	}
}
