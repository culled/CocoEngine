#include "ViewportCameraControllerComponent.h"

#include <Coco/Input/InputService.h>

#include <Coco/ECS/Components/Transform3DComponent.h>

using namespace Coco::ECS;
using namespace Coco::Input;

namespace Coco
{
	const float ViewportCameraControllerComponent::MinMoveSpeed = 0.01f;
	const float ViewportCameraControllerComponent::MaxMoveSpeed = 100;

	ViewportCameraControllerComponent::ViewportCameraControllerComponent(const Entity& entity) :
		EntityComponent(entity),
		_lookSensitivity(0.5f),
		_moveSpeed(2.0),
		_panSpeed(2.0),
		_invertPan{false}
	{}

	void ViewportCameraControllerComponent::Navigate()
	{
		InputService& input = *InputService::Get();

		Mouse& mouse = input.GetMouse();

		Transform3DComponent& transform = GetOwner().GetComponent<Transform3DComponent>();
		bool shouldFly = mouse.IsButtonPressed(MouseButton::Right);

		Vector2Int mouseDelta = mouse.GetMoveDelta();
		int scrollDelta = mouse.GetScrollWheelDelta().Y;
		Keyboard& keyboard = input.GetKeyboard();

		if (shouldFly)
		{
			Vector3 eulerAngles = transform.GetEulerAngles(TransformSpace::Parent);
			eulerAngles.X = Math::Clamp(eulerAngles.X - mouseDelta.Y * _lookSensitivity * 0.01, Math::DegToRad(-90.0), Math::DegToRad(90.0));
			eulerAngles.Y -= mouseDelta.X * _lookSensitivity * 0.01;

			transform.SetRotation(Quaternion(eulerAngles), TransformSpace::Parent);

			if (scrollDelta != 0)
			{
				_moveSpeed = Math::Clamp(_moveSpeed + (_moveSpeed * scrollDelta * 0.2f), MinMoveSpeed, MaxMoveSpeed);
			}

			Vector3 velocity = Vector3::Zero;

			if (keyboard.IsKeyPressed(KeyboardKey::W))
				velocity += Vector3::Forward;

			if (keyboard.IsKeyPressed(KeyboardKey::S))
				velocity += Vector3::Backward;

			if (keyboard.IsKeyPressed(KeyboardKey::D))
				velocity += Vector3::Right;

			if (keyboard.IsKeyPressed(KeyboardKey::A))
				velocity += Vector3::Left;

			if (keyboard.IsKeyPressed(KeyboardKey::E))
				velocity += Vector3::Up;

			if (keyboard.IsKeyPressed(KeyboardKey::Q))
				velocity += Vector3::Down;

			const TickInfo& currentTick = MainLoop::Get()->GetCurrentTick();
			transform.Translate(velocity * (currentTick.UnscaledDeltaTime * _moveSpeed), TransformSpace::Self);
		}
		else
		{
			Vector3 pan(-mouseDelta.X, mouseDelta.Y, 0.0);
			if (_invertPan.at(0))
				pan.X = -pan.X;

			if (_invertPan.at(1))
				pan.Y = -pan.Y;

			transform.Translate(pan * (_panSpeed * 0.01), TransformSpace::Self);
		}
	}
}