#pragma once

#include <Coco/Core/Core.h>

#include <Coco/ECS/Components/ScriptComponent.h>
#include <Coco/Core/Resources/ResourceTypes.h>

using namespace Coco;
using namespace Coco::ECS;

class Block : public ScriptComponent
{
private:
	static ResourceID _blockMeshID;

	Size _size = Size(2.0, 0.5);
	Color _color = Color::Yellow;

public:
	Block(const EntityID& owner, const Vector3& position, const ResourceID& materialID);
	~Block() final = default;
};
