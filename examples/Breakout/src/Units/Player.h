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

class Player : public ScriptComponent
{
private:
	bool _canMove = false;
	Size _size = Size(4, 0.5);

	double _moveSpeed = 50.0;
	double _moveAcceleration = 320.0;
	double _currentVelocity = 0.0;
	double _maxPositionX = 15.0;
	double _positionY = -8.0;

	Color _color = Color(0.79, 0.27, 0.26);

	Ref<Mesh> _mesh;
	Ref<Material> _material;

public:
	Player(const EntityID& owner);
	~Player() final;

	Rect GetRect() const;

protected:
	void Tick(double deltaTime) override;

private:
	void Move(double deltaTime);

	bool HandleGameStarted();
	bool HandleGameEnded();
};