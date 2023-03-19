#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Logging/Logger.h>
#include "EngineServiceExceptions.h"
#include <Coco/Core/MainLoop/MainLoopTickListener.h>

namespace Coco
{
	class Engine;

	/// <summary>
	/// A class that provides a service to the engine
	/// </summary>
	class COCOAPI EngineService
	{
	protected:
		Engine* Engine;
		List<Ref<MainLoopTickListener>> TickListeners;

	protected:
		EngineService(Coco::Engine* engine);

	public:
		virtual ~EngineService() = default;

		/// <summary>
		/// Gets this service's logger
		/// </summary>
		/// <returns>This service's logger</returns>
		virtual Logging::Logger* GetLogger() const noexcept;

		/// <summary>
		/// Called when the service should start executing
		/// </summary>
		void Start();

	protected:
		/// <summary>
		/// Called when the service should start executing
		/// </summary>
		virtual void StartImpl() {}

		/// <summary>
		/// Sets a tick listener to be registered when this service starts
		/// </summary>
		/// <param name="instance">The instance</param>
		/// <param name="handlerFunction">The tick function</param>
		/// <param name="priority">The priority of the listener</param>
		template<typename ObjectT>
		void RegisterTickListener(ObjectT* instance, void(ObjectT::* handlerFunction)(double), int priority)
		{
			TickListeners.Add(CreateRef<MainLoopTickListener>(instance, handlerFunction, priority));
		}
	};
}