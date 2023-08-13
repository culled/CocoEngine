#include "Block.h"

#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include <Coco/Rendering/MeshPrimitives.h>

#include <Coco/Core/Engine.h>
#include "../App.h"

using namespace Coco;
using namespace Coco::ECS;

ResourceID Block::_blockMeshID = ResourceID::Nil;

Block::Block(const EntityID& owner, const Vector3& position, const ResourceID& materialID, double speedValue) : ScriptComponent(owner),
	_speedValue(speedValue)
{
	ECSService* ecs = ECSService::Get();

	ecs->AddComponent<TransformComponent>(owner, position, Quaternion::Identity, Vector3::One);

	Ref<Rendering::Mesh> mesh;
	if (_blockMeshID == ResourceID::Nil)
	{
		mesh = Rendering::MeshPrimitives::CreateXYPlane("Block", _size);
		mesh->UploadData();

		_blockMeshID = mesh->ID;
	}
	else
	{
		mesh = Engine::Get()->GetResourceLibrary()->GetResource<Rendering::Mesh>(_blockMeshID);
	}

	ecs->AddComponent<MeshRendererComponent>(owner, mesh, Engine::Get()->GetResourceLibrary()->GetResource<Rendering::Material>(materialID));
}

bool Block::CheckCollision(const Rect& other, Vector2& hitPoint, Vector2& hitNormal)
{
	TransformComponent& t = ECSService::Get()->GetComponent<TransformComponent>(Owner);
	Vector3 p = t.GetGlobalPosition();
	Rect r(Vector2(p.X - _size.Width * 0.5, p.Y - _size.Height * 0.5), _size);

	if (r.Intersects(other))
	{
		Rect::RectangleSide side;
		hitPoint = r.GetClosestPoint(other, &side);
		hitNormal = Rect::GetNormalOfSide(side);

		return true;
	}

	return false;
}
