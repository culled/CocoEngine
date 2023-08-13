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
	double _moveSpeed = 8.0;

	Vector3 _startPosition = Vector3(0.0, -7.0, 0.0);
	Vector3 _moveDirection = Vector3::Zero;

	Color _color = Color(0.79, 0.27, 0.26);

	Ref<Mesh> _mesh;
	Ref<Material> _material;

public:
	Ball(const EntityID& owner);
	~Ball() final;

	Rect GetRect() const;

	void Bounce(const Vector2& hitPoint, const Vector2& normal);
	void SpeedUp(double newSpeed);

protected:
	void Tick(double deltaTime) override;

private:
	void UpdateMovement(double dt);

	bool HandleGameStarted();
	bool HandleGameEnded();
};