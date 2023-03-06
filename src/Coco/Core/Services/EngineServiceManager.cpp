#include "EngineServiceManager.h"
#include "EngineService.h"

namespace Coco
{
	EngineServiceManager::~EngineServiceManager()
	{
		// Destroy services in reverse order that they were added
		for (int64_t i = _services.Count() - 1; i >= 0; i--)
		{
			_services[i].reset();
		}

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