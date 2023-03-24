#include "EngineServiceManager.h"
#include "EngineService.h"

namespace Coco
{
	EngineServiceManager::EngineServiceManager(Coco::Engine* engine) : Engine(engine)
	{}

	EngineServiceManager::~EngineServiceManager()
	{
		// Destroy services in reverse order that they were added
		std::for_each(_services.rbegin(), _services.rend(), [](auto& serviceKVP) { serviceKVP.second.reset(); });

		_services.clear();
	}

	void EngineServiceManager::Start()
	{
		// Start all services
		for (const auto& serviceKVP : _services)
		{
			serviceKVP.second->Start();
		}

		_isStarted = true;
	}
}