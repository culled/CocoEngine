#pragma once

#include <Coco/ECS/Entity.h>

namespace Coco
{
	class ComponentUI
	{
	public:
		virtual ~ComponentUI() = default;
		void Draw(ECS::Entity& entity);

	protected:
		virtual const char* GetHeader() const = 0;
		virtual void DrawImpl(ECS::Entity& entity) = 0;
	};
}