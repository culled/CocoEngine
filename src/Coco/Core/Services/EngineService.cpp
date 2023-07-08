#include "EngineService.h"

#include <Coco/Core/Engine.h>

namespace Coco
{
	Logging::Logger* EngineService::GetLogger() noexcept
	{
		return Engine::Get()->GetLogger();
	}
}
