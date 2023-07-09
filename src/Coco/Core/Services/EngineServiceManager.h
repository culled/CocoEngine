#pragma once
#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include "EngineService.h"

#include <typeinfo>

namespace Coco
{
	/// @brief Manages engine services
	class COCOAPI EngineServiceManager
	{
		friend class Engine;

	private:
		bool _isStarted = false;
		Map<std::type_index, ManagedRef<EngineService>> _services;

	public:
		EngineServiceManager() = default;
		~EngineServiceManager();

		EngineServiceManager(const EngineServiceManager&) = delete;
		EngineServiceManager(EngineServiceManager&&) = delete;
		EngineServiceManager operator=(const EngineServiceManager&) = delete;
		EngineServiceManager operator=(EngineServiceManager&&) = delete;

		/// @brief Creates a service and registers it
		/// @tparam ServiceType The type of service
		/// @tparam ...Args The types of arguments for the service's constructor
		/// @param ...args Arguments to pass to the service's constructor
		/// @return The created service
		template<typename ServiceType, typename ... Args>
		ServiceType* CreateService(Args&& ... args)
		{
			static_assert(std::is_base_of<EngineService, ServiceType>::value, "Can only create services derived from EngineService");

			std::type_index type = typeid(ServiceType);

			if (!_services.contains(type))
			{
				auto it = _services.emplace(type, CreateManagedRef<ServiceType>(std::forward<Args>(args)...)).first;

				if (_isStarted)
					(*it).second->Start();
			}

			return static_cast<ServiceType*>(_services.at(type).Get());
		}

		/// @brief Checks if a service of the given type has been created
		/// @param serviceType The type of service (e.g typeid(ServiceClass)) 
		/// @return True if the service exists
		bool HasService(const std::type_info& serviceType) const noexcept;

		/// @brief Gets a service. WARNING: only use this if you are sure the service exists
		/// @tparam ServiceType The type of service
		/// @return A pointer to the service instance
		template<typename ServiceType>
		ServiceType* GetService()
		{
			return static_cast<ServiceType*>(_services.at(typeid(ServiceType)).Get());
		}

		/// @brief Tries to find a service and returns it if it exists
		/// @tparam ServiceType The type of service
		/// @param servicePtr A pointer that will be assigned to the service if it is found
		/// @return True if the service exists
		template<typename ServiceType>
		bool TryGetService(ServiceType*& servicePtr) noexcept
		{
			const auto& it = _services.find(typeid(ServiceType));
		
			if (it != _services.cend())
			{
				servicePtr = static_cast<ServiceType*>((*it).second.Get());
				return true;
			}
		
			return false;
		}

	private:
		/// @brief Starts all services
		void Start();
	};
}

