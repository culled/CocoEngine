#pragma once

#include <Coco/ECS/Components/ScriptComponent.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Color.h>

using namespace Coco;
using namespace Coco::ECS;
using namespace Coco::Rendering;

class Ball : public ScriptComponent
{
private:
	Size _size = Size(0.5, 0.5);
	double _moveSpeed = 5.0;

	Vector3 _startPosition = Vector3(0.0, -7.0, 0.0);
	Vector3 _velocity = Vector3::Zero;

	Color _color = Color::Magenta;

	Ref<Mesh> _mesh;
	Ref<Material> _material;

public:
	Ball(const EntityID& owner);
	~Ball() final;

protected:
	void Tick(double deltaTime) override;

private:
	bool HandleGameStarted();
	bool HandleGameEnded();
};