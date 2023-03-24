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

	void EngineService::Start()
	{
		StartImpl();

		MainLoop* loop = ServiceManager->Engine->GetMainLoop();

		for (const Ref<MainLoopTickListener>& listener : TickListeners)
			loop->AddTickListener(listener);
	}
}
