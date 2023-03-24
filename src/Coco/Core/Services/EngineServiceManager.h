#pragma once
#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include "EngineService.h"

#include <typeinfo>

namespace Coco
{
	class Engine;

	/// @brief Manages services for the engine
	class COCOAPI EngineServiceManager
	{
	public:
		/// @brief A pointer to the engine
		Engine* const Engine;

	private:
		bool _isStarted = false;
		Map<const char*, Managed<EngineService>> _services;

	public:
		EngineServiceManager(Coco::Engine* engine);
		~EngineServiceManager();

		EngineServiceManager(const EngineServiceManager&) = delete;
		EngineServiceManager(EngineServiceManager&&) = delete;

		EngineServiceManager operator=(const EngineServiceManager&) = delete;
		EngineServiceManager operator=(EngineServiceManager&&) = delete;

		/// @brief Creates a service and registers it
		/// @tparam ServiceType 
		/// @tparam ...Args 
		/// @param ...args Arguments to pass to the service's constructor
		/// @return The created service
		template<typename ServiceType, typename ... Args>
		ServiceType* CreateService(Args&& ... args)
		{
			static_assert(std::is_base_of<EngineService, ServiceType>::value, "Can only create services derived from EngineService");

			const char* typeName = typeid(ServiceType).name();

			if (!_services.contains(typeName))
			{
				auto it = _services.emplace(typeName, CreateManaged<ServiceType>(this, std::forward<Args>(args)...)).first;

				if (_isStarted)
					(*it).second->Start();
			}

			return static_cast<ServiceType*>(_services.at(typeName).get());
		}

		/// @brief Tries to find a service and returns it
		/// @tparam ServiceType 
		/// @param servicePtr A pointer that will be assigned to the service if it is found
		/// @return True if a service of the given type has been registered
		template<typename ServiceType>
		bool TryFindService(ServiceType*& servicePtr) const noexcept
		{
			const auto& it = _services.find(typeid(ServiceType).name());

			if (it != _services.cend())
			{
				servicePtr = static_cast<ServiceType*>((*it).second.get());
				return true;
			}

			servicePtr = nullptr;
			return false;
		}

		/// @brief Starts all services
		void Start();
	};
}

