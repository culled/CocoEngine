#pragma once

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Components/Rendering/CameraComponent.h>

namespace Coco
{
	class CameraComponentUI
	{
	public:
		static void DrawInspectorUI(ECS::CameraComponent& camera);
	};
}
