#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/MainLoop/MainLoopTickListener.h>
#include "EngineServiceExceptions.h"

namespace Coco
{
	class EngineServiceManager;

	/// @brief Provides a service to the engine
	class COCOAPI EngineService
	{
	public:
		/// @brief A pointer to the servie manager
		EngineServiceManager* const ServiceManager;

	protected:
		/// @brief Tick listeners for this service
		List<Ref<MainLoopTickListener>> TickListeners;

	protected:
		EngineService(EngineServiceManager* serviceManager);

	public:
		virtual ~EngineService() = default;

		/// @brief Gets this service's logger
		/// @return This service's logger
		virtual Logging::Logger* GetLogger() const noexcept;

		/// @brief Starts this service
		void Start();

	protected:
		/// @brief Called when the service should start
		virtual void StartImpl() {}

		/// @brief Sets a tick listener to be registered when this service starts
		/// @tparam ObjectT 
		/// @param instance The instance
		/// @param handlerFunction The tick function
		/// @param priority The priority of the listener
		template<typename ObjectT>
		void RegisterTickListener(ObjectT* instance, void(ObjectT::* handlerFunction)(double), int priority)
		{
			TickListeners.Add(CreateRef<MainLoopTickListener>(instance, handlerFunction, priority));
		}
	};
}