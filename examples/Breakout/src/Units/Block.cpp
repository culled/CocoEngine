#include "Block.h"

#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include <Coco/Rendering/MeshPrimitives.h>

#include <Coco/Core/Engine.h>
#include "../App.h"

using namespace Coco;
using namespace Coco::ECS;

ResourceID Block::_blockMeshID = ResourceID::Nil;

Block::Block(const EntityID& owner, const Vector3& position, const ResourceID& materialID) : ScriptComponent(owner)
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
