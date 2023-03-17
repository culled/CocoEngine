#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Logging/Logger.h>
#include "EngineServiceExceptions.h"

namespace Coco
{
	/// <summary>
	/// A class that provides a service to the engine
	/// </summary>
	class COCOAPI EngineService
	{
	public:
		virtual ~EngineService() = default;

		/// <summary>
		/// Gets this service's logger
		/// </summary>
		/// <returns>This service's logger</returns>
		virtual Logging::Logger* GetLogger() const noexcept = 0;

		/// <summary>
		/// Called when the service should start executing
		/// </summary>
		virtual void Start() = 0;
	};
}