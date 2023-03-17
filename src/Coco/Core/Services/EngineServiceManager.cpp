#include "EngineServiceManager.h"
#include "EngineService.h"

namespace Coco
{
	EngineServiceManager::~EngineServiceManager()
	{
		// Destroy services in reverse order that they were added
		std::for_each(_services.rbegin(), _services.rend(), [](Managed<EngineService>& service) { service.reset(); });

		_services.Clear();
	}

	void EngineServiceManager::Start()
	{
		// Start all services
		for (const Managed<EngineService>& service : _services)
		{
			service->Start();
		}

		_isStarted = true;
	}
}