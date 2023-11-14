#include "MovingBox.h"

#include <Coco/ECS/Components/Transform3DComponent.h>

using namespace Coco::ECS;

void MovingBox::OnUpdate(const TickInfo& tickInfo)
{
	double t = tickInfo.Time;

	Transform3DComponent& transform = GetComponent<Transform3DComponent>();
	transform.SetRotation(Quaternion(Vector3(Math::Sin(t), Math::Cos(t * 0.8 + 20.0), Math::Sin(t * 1.2 - 30.0))), TransformSpace::Global);
}
