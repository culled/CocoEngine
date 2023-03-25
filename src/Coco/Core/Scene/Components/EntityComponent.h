#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	class SceneEntity;

	/// @brief A generic component that can be added to entities
	class COCOAPI EntityComponent
	{
	protected:
		SceneEntity* Entity;

	protected:
		EntityComponent(SceneEntity* entity);

	public:
		virtual ~EntityComponent() = default;
	};
}