#include "Corepch.h"
#include "ServiceManager.h"

namespace Coco
{
	ServiceManager::~ServiceManager()
	{
		// Destroy services in the reverse order they were added
		for (auto it = _serviceRegisterOrder.rbegin(); it != _serviceRegisterOrder.rend(); it++)
			_services.erase(*it);
	}

	bool ServiceManager::HasService(const std::type_info& type)
	{
		return _services.contains(type);
	}

	EngineService* ServiceManager::GetService(const std::type_info& type)
	{
		return _services.at(type).get();
	}
}