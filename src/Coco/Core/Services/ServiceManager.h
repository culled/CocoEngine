#pragma once

#include "../Corepch.h"
#include "../Types/Singleton.h"
#include "../Types/Refs.h"
#include "EngineService.h"

namespace Coco
{
	/// @brief Manages EngineServices for the Engine
	class ServiceManager : public Singleton<ServiceManager>
	{
	private:
		std::unordered_map<std::type_index, UniqueRef<EngineService>> _services;
		std::vector<std::type_index> _serviceRegisterOrder;

	public:
		~ServiceManager();

		/// @brief Creates a service and registers it to this manager
		/// @tparam ServiceType The type of service to create
		/// @tparam ...Args The type of arguments
		/// @param ...args The arguments to pass to the service's constructor
		/// @return The created service
		template<typename ServiceType, typename ... Args>
		ServiceType& CreateService(Args&& ... args)
		{
			auto result = _services.try_emplace(typeid(ServiceType), CreateUniqueRef<ServiceType>(std::forward<Args>(args)...));

			if(result.second)
			{
				_serviceRegisterOrder.push_back(typeid(ServiceType));
			}

			UniqueRef<EngineService>& service = result.first->second;
			return static_cast<ServiceType&>(*service);
		}

		/// @brief Checks if a service of the given type exists
		/// @param type The type of service
		/// @return True if a service of the given type exists
		bool HasService(const std::type_info& type);

		/// @brief Checks if a service of the given type exists
		/// @tparam ServiceType The type of service
		/// @return True if a service of the given type exists
		template<typename ServiceType>
		bool HasService()
		{
			return HasService(typeid(ServiceType));
		}

		/// @brief Gets a service of the given type.
		/// NOTE: if you're unsure if the service exists, call Has() to check if the service exists
		/// @param type The type of service to get
		/// @return The service
		EngineService& GetService(const std::type_info& type);

		/// @brief Gets a service of the given type.
		/// NOTE: if you're unsure if the service exists, call Has() to check if the service exists 
		/// @tparam ServiceType The type of service
		/// @return The service
		template<typename ServiceType>
		ServiceType& GetService()
		{
			return static_cast<ServiceType&>(GetService(typeid(ServiceType)));
		}
	};
}