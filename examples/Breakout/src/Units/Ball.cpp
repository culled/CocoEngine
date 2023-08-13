#include "Ball.h"

#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/MeshPrimitives.h>
#include "../App.h"

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
	TransformComponent& transform = ECSService::Get()->GetComponent<TransformComponent>(Owner);
	Vector3 position = transform.GetGlobalPosition();
	position += _velocity * deltaTime;
	transform.SetGlobalPosition(position);
}

bool Ball::HandleGameStarted()
{
	_velocity = Vector3::Forwards * _moveSpeed;

	return false;
}

bool Ball::HandleGameEnded()
{
	_velocity = Vector3::Zero;

	return false;
}
