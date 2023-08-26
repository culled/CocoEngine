#include "Ball.h"

#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/MeshPrimitives.h>
#include "../App.h"
#include <Coco/Core/Math/Random.h>

Ball::Ball(const EntityID& owner) : ScriptComponent(owner)
{
	ECSService* ecs = ECSService::Get();
	auto& transform = ecs->AddComponent<TransformComponent>(owner);
	transform.SetGlobalPosition(_startPosition);

	ResourceLibrary* library = Engine::Get()->GetResourceLibrary();
	App* app = App::Get();

	_material = library->CreateResource<Material>("Material::Ball", App::Get()->GetBasicShader());
	_material->SetColor("_BaseColor", _color);
	_material->SetTexture("_MainTex", app->GetRenderingService()->GetDefaultDiffuseTexture());

	_mesh = MeshPrimitives::CreateXYPlane("Ball Mesh", _size);
	_mesh->UploadData();

	ecs->AddComponent<MeshRendererComponent>(owner, _mesh, _material);

	app->OnStartPlaying.AddHandler(this, &Ball::HandleGameStarted);
	app->OnStopPlaying.AddHandler(this, &Ball::HandleGameEnded);
}

Ball::~Ball()
{
	App* app = App::Get();
	app->OnStartPlaying.RemoveHandler(this, &Ball::HandleGameStarted);
	app->OnStopPlaying.RemoveHandler(this, &Ball::HandleGameEnded);
}

void Ball::Tick(double deltaTime)
{
	UpdateMovement(deltaTime);
}

Rect Ball::GetRect() const
{
	TransformComponent& transform = ECSService::Get()->GetComponent<TransformComponent>(Owner);
	Vector3 position = transform.GetGlobalPosition();

	return Rect(Vector2(position.X - _size.Width * 0.5, position.Y - _size.Height * 0.5), _size);
}

void Ball::Bounce(const Vector2& hitPoint, const Vector2& normal)
{
	Vector3 vec3Normal(normal.X, normal.Y, 0.0);

	if (_moveDirection.Dot(vec3Normal) < 0.0)
	{
		_moveDirection = _moveDirection.Reflect(vec3Normal);
	}

	Vector2 p = GetRect().GetClosestPoint(hitPoint);
	double offset = Vector2::DistanceBetween(p, hitPoint);

	TransformComponent& transform = ECSService::Get()->GetComponent<TransformComponent>(Owner);
	transform.SetGlobalPosition(transform.GetGlobalPosition() + vec3Normal * offset);
}

void Ball::SpeedUp(double newSpeed)
{
	_moveSpeed = Math::Max(newSpeed, _moveSpeed);
}

void Ball::UpdateMovement(double dt)
{
	TransformComponent& transform = ECSService::Get()->GetComponent<TransformComponent>(Owner);
	Vector3 position = transform.GetGlobalPosition();
	position += _moveDirection * _moveSpeed * dt;
	transform.SetGlobalPosition(position);
}

bool Ball::HandleGameStarted()
{
	Random rand;
	//_moveDirection = Vector3(rand.RandomRange(-1.0, 1.0), 1.0, 0.0);
	_moveDirection = Vector3(0.1, 1.0, 0.0);
	_moveDirection.Normalize();

	return false;
}

bool Ball::HandleGameEnded()
{
	_moveDirection = Vector3::Zero;

	return false;
}
