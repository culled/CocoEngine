#pragma once
#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include "EngineService.h"

#include <typeinfo>

namespace Coco
{
	class Engine;

	/// <summary>
	/// Manages services for the engine
	/// </summary>
	class COCOAPI EngineServiceManager
	{
	private:
		Engine* _engine;
		bool _isStarted = false;
		Map<const char*, Managed<EngineService>> _services;

	public:
		EngineServiceManager(Engine* engine);
		~EngineServiceManager();

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

			const char* typeName = typeid(T).name();

			if (!_services.contains(typeName))
			{
				auto it = _services.emplace(typeName, CreateManaged<T>(_engine, std::forward<Args>(args)...)).first;

				if (_isStarted)
					(*it).second->Start();
			}

			return static_cast<T*>(_services.at(typeName).get());
		}

		/// <summary>
		/// Tries to find a registered service and returns it
		/// </summary>
		/// <param name="servicePtr">A pointer that will be assigned to the service if it is found</param>
		/// <returns>True if a service of the given type has been registered</returns>
		template<typename T>
		bool TryFindService(T*& servicePtr) const noexcept
		{
			const auto& it = _services.find(typeid(T).name());

			if (it != _services.cend())
			{
				servicePtr = static_cast<T*>((*it).second.get());
				return true;
			}

			servicePtr = nullptr;
			return false;
		}

		/// <summary>
		/// Starts all services
		/// </summary>
		void Start();
	};
}

