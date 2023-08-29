#include "Player.h"

#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include <Coco/Core/Engine.h>
#include <Coco/Rendering/MeshPrimitives.h>
#include <Coco/Input/InputService.h>
#include "../App.h"

Player::Player(const EntityID& owner) : ScriptComponent(owner)
{
	ECSService* ecs = ECSService::Get();
	auto& transform = ecs->AddComponent<TransformComponent>(owner);
	transform.SetGlobalPosition(Vector3(0.0, _positionY, 0.0));

	ResourceLibrary* library = Engine::Get()->GetResourceLibrary();
	App* app = App::Get();

	_material = library->CreateResource<Material>("Material::Player", App::Get()->GetBasicShader());
	_material->SetColor("_BaseColor", _color);
	_material->SetTexture("_MainTex", app->GetRenderingService()->GetDefaultDiffuseTexture());

	_mesh = MeshPrimitives::CreateXYPlane("Player Mesh", _size);
	_mesh->EnsureChannels(true, true, true, true);
	_mesh->UploadData();

	ecs->AddComponent<MeshRendererComponent>(owner, _mesh, _material);

	app->OnStartPlaying.AddHandler(this, &Player::HandleGameStarted);
	app->OnStopPlaying.AddHandler(this, &Player::HandleGameEnded);
}

Player::~Player()
{
	App* app = App::Get();
	app->OnStartPlaying.RemoveHandler(this, &Player::HandleGameStarted);
	app->OnStopPlaying.RemoveHandler(this, &Player::HandleGameEnded);
}

Rect Player::GetRect() const
{
	TransformComponent& transform = ECSService::Get()->GetComponent<TransformComponent>(Owner);
	Vector3 position = transform.GetGlobalPosition();

	return Rect(Vector2(position.X - _size.Width * 0.5, position.Y - _size.Height * 0.5), _size);
}

void Player::Tick(double deltaTime)
{
	if (_canMove)
		Move(deltaTime);
}

void Player::Move(double deltaTime)
{
	using namespace Coco::Input;

	// Get the player input
	double currentInput = 0.0;

	InputService* input = App::Get()->GetInputService();

	if (input->GetKeyboard()->IsKeyPressed(KeyboardKey::D))
		currentInput = 1.0;
	else if (input->GetKeyboard()->IsKeyPressed(KeyboardKey::A))
		currentInput = -1.0;

	// Apply the input to our velocity
	double targetVelocity = currentInput * _moveSpeed;
	double velocityDiff = targetVelocity - _currentVelocity;
	double acceleration = Math::Clamp(velocityDiff, -_moveAcceleration, _moveAcceleration);

	_currentVelocity += acceleration * deltaTime;

	TransformComponent& transform = ECSService::Get()->GetComponent<TransformComponent>(Owner);
	Vector3 position = transform.GetGlobalPosition();
	position.X += _currentVelocity * deltaTime;
	position.Y = _positionY;

	// Clamp the position
	if (position.X < -_maxPositionX || position.X > _maxPositionX)
	{
		position.X = Math::Clamp(position.X, -_maxPositionX, _maxPositionX);
		_currentVelocity = 0.0;
	}

	transform.SetGlobalPosition(position);
}

bool Player::HandleGameStarted()
{
	_canMove = true;

	return false;
}

bool Player::HandleGameEnded()
{
	_canMove = false;

	return false;
}
