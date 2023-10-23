#pragma once

namespace Coco
{
	/// @brief Provides a service to the Engine
	class EngineService
	{
		friend class ServiceManager;

	public:
		virtual ~EngineService() = default;
	};
}