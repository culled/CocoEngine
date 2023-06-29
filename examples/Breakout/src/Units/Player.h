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

class Player : public AttachedScript
{
private:
	Size _size = Size(4, 0.5);

	double _moveSpeed = 50.0;
	double _moveAcceleration = 240.0;
	double _currentVelocity = 0.0;
	double _maxPositionX = 15.0;
	double _positionY = -8.0;

	Color _playerColor = Color::White;

	Ref<Mesh> _mesh;
	Ref<Material> _material;

public:
	Player() = default;
	~Player() final;

protected:
	void Start() override;
	void Tick(double deltaTime) override;
};