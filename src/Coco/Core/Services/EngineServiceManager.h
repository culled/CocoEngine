#pragma once
#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include "EngineService.h"

namespace Coco
{
	/// <summary>
	/// Manages services for the engine
	/// </summary>
	class COCOAPI EngineServiceManager
	{
	private:
		bool _isStarted = false;
		List<Managed<EngineService>> _services;

	public:
		~EngineServiceManager();

		EngineServiceManager() = default;
		EngineServiceManager(const EngineServiceManager&) = delete;
		EngineServiceManager(EngineServiceManager&&) = delete;

		EngineServiceManager operator=(const EngineServiceManager&) = delete;
		EngineServiceManager operator=(EngineServiceManager&&) = delete;

		/// <summary>
		/// Creates a service and registers it
		/// </summary>
		/// <typeparam name="T">The type of service to create</typeparam>
		/// <typeparam name="...Args">Arguement types</typeparam>
		/// <param name="args">Arguments to pass to the service's constructor</param>
		/// <returns>The created service</returns>
		template<typename T, typename ... Args>
		T* CreateService(Args&& ... args)
		{
			static_assert(std::is_base_of<EngineService, T>::value, "Can only create services derived from EngineService");

			// TODO: prevent duplicate services
			T* service = new T(std::forward<Args>(args)...);

			RegisterService(service);

			return service;
		}

		/// <summary>
		/// Tries to find a registered service and returns it
		/// </summary>
		/// <param name="servicePtr">A pointer that will be assigned to the service if it is found</param>
		/// <returns>True if a service of the given type has been registered</returns>
		template<typename T>
		bool TryFindService(T** servicePtr) const noexcept
		{
			for (const Managed<EngineService>& service : _services)
			{
				if (T* activeService = dynamic_cast<T*>(service.get()))
				{
					*servicePtr = activeService;
					return true;
				}
			}

			servicePtr = nullptr;
			return false;
		}

		/// <summary>
		/// Starts all services
		/// </summary>
		void Start();

	private:
		/// <summary>
		/// Registers a service to this manager
		/// </summary>
		/// <typeparam name="T">The type of service to register</typeparam>
		/// <param name="service">The service to register</param>
		template<typename T>
		void RegisterService(T* service)
		{
			static_assert(std::is_base_of<EngineService, T>::value, "Can only register services derived from EngineService");

			// TODO: prevent duplicate services
			_services.Add(Managed<EngineService>(service));

			if (_isStarted)
				service->Start();
		}
	};
}

