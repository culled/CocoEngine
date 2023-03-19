#include "EngineService.h"
#include <Coco/Core/Engine.h>

namespace Coco
{
	EngineService::EngineService(Coco::Engine* engine) : 
		Engine(engine)
	{}

	Logging::Logger* EngineService::GetLogger() const noexcept
	{
		return Engine->GetLogger();
	}

	void EngineService::Start()
	{
		StartImpl();

		MainLoop* loop = Engine->GetMainLoop();

		for (Ref<MainLoopTickListener>& listener : TickListeners)
			loop->AddTickListener(listener);
	}
}
