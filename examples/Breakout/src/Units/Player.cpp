#include "Player.h"

#include <Coco/ECS/Components/TransformComponent.h>
#include <Coco/ECS/Components/MeshRendererComponent.h>
#include <Coco/Core/Engine.h>
#include <Coco/Core/Resources/ResourceLibrary.h>
#include <Coco/Rendering/MeshPrimitives.h>
#include <Coco/Input/InputService.h>
#include "../App.h"

Player::Player(EntityID owner) : ScriptComponent(owner)
{
	ECSService* ecs = ECSService::Get();
	auto& transform = ecs->AddComponent<TransformComponent>(owner);
	transform.SetGlobalPosition(Vector3(0.0, _positionY, 0.0));

	ResourceLibrary* library = Engine::Get()->GetResourceLibrary();

	_material = library->CreateResource<Material>("Material::Player", App::Get()->GetBasicShader());
	_material->SetVector4("_BaseColor", _playerColor);
	_material->SetTexture("_MainTex", App::Get()->GetRenderingService()->GetDefaultDiffuseTexture());

	//_mesh = MeshPrimitives::CreateFromVertices(vertexPositions, vertexUVs, vertexIndices);
	_mesh = MeshPrimitives::CreateXYPlane("Player Mesh", _size);
	//_mesh = MeshPrimitives::CreateBox(Vector3::One);
	_mesh->UploadData();

	ecs->AddComponent<MeshRendererComponent>(owner, _mesh, _material);
}

Player::~Player()
{
}

void Player::Tick(double deltaTime)
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

	TransformComponent& transform = ECSService::Get()->GetComponent<TransformComponent>(_owner);
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
