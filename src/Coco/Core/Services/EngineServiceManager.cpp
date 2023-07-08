#include "EngineServiceManager.h"
#include "EngineService.h"

namespace Coco
{
	EngineServiceManager::~EngineServiceManager()
	{
		// Destroy services in reverse order that they were added
		std::for_each(_services.rbegin(), _services.rend(), [](auto& serviceKVP) { serviceKVP.second.Reset(); });

		_services.clear();
	}

	bool EngineServiceManager::HasService(const std::type_info& serviceType) const noexcept
	{
		return _services.contains(serviceType);
	}

	void EngineServiceManager::Start()
	{
		// Start all services
		for (auto& serviceKVP : _services)
		{
			serviceKVP.second->Start();
		}

		_isStarted = true;
	}
}