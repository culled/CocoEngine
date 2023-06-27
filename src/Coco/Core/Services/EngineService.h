#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Logging/Logger.h>
#include "EngineServiceExceptions.h"

namespace Coco
{
	class EngineServiceManager;

	/// @brief Provides a service to the engine
	class COCOAPI EngineService
	{
	public:
		/// @brief A pointer to the service manager
		EngineServiceManager* const ServiceManager;

	protected:
		EngineService(EngineServiceManager* serviceManager);

	public:
		virtual ~EngineService() = default;

		/// @brief Gets this service's logger
		/// @return This service's logger
		virtual Logging::Logger* GetLogger() const noexcept;

		/// @brief Starts this service
		virtual void Start() {}
	};
}