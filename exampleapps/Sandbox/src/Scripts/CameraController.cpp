#include "CameraController.h"

#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/ECS/Components/Transform3DComponent.h>

#include <Coco/Core/Engine.h>

using namespace Coco::Input;
using namespace Coco::Windowing;
using namespace Coco::ECS;

CameraController::CameraController() :
    _mouseSensitivity(0.005),
    _moveInput(Vector3::Zero),
    _isLooking(false),
    _mouseStateChangedHandler(this, &CameraController::OnMouseStateUpdate),
    _keyboardStateChangedHandler(this, &CameraController::OnKeyboardStateUpdate)
{
}

void CameraController::OnCreate()
{
    InputService& input = *InputService::Get();
    Ref<UnhandledInputLayer> unhandledInput = input.GetUnhandledInputLayer();
    _mouseStateChangedHandler.Connect(unhandledInput->OnMouseStateChanged);
    _keyboardStateChangedHandler.Connect(unhandledInput->OnKeyboardStateChanged);

    CocoInfo("CameraController::OnCreate")
}

void CameraController::OnStart()
{
    CocoInfo("CameraController::OnStart")
}

void CameraController::OnUpdate(const Coco::TickInfo& tickInfo)
{
    Transform3DComponent& transform = GetComponent<Transform3DComponent>();
    transform.Translate(_moveInput * tickInfo.DeltaTime, TransformSpace::Self);
}

void CameraController::OnDestroy()
{
    _mouseStateChangedHandler.DisconnectAll();
    _keyboardStateChangedHandler.DisconnectAll();

    CocoInfo("CameraController::OnDestroy")
}

bool CameraController::OnMouseStateUpdate(const Input::MouseStateChange& state)
{
    bool handled = false;

    if (state.Button.has_value() && state.Button.value() == MouseButton::Right)
    {
        Ref<Window> mainWindow = WindowService::Get()->GetMainWindow();

        if (state.IsButtonPressed)
        {
            mainWindow->SetCursorConfineMode(CursorConfineMode::LockedCenter);
            mainWindow->SetCursorVisibility(false);
            _isLooking = true;
        }
        else
        {
            mainWindow->SetCursorConfineMode(CursorConfineMode::None);
            mainWindow->SetCursorVisibility(true);
            _isLooking = false;
        }

        handled = true;
    }

    if (state.MoveDelta.has_value() && _isLooking)
    {
        Transform3DComponent& transform = GetComponent<Transform3DComponent>();

        Vector2Int mouseDelta = state.MoveDelta.value();
        Vector3 eulerAngles = transform.GetEulerAngles(TransformSpace::Parent);
        eulerAngles.X = Math::Clamp(eulerAngles.X - mouseDelta.Y * _mouseSensitivity, Math::DegToRad(-90.0), Math::DegToRad(90.0));
        eulerAngles.Y -= mouseDelta.X * _mouseSensitivity;

        transform.SetEulerAngles(eulerAngles, TransformSpace::Parent);

        handled = true;
    }

    return handled;
}

bool CameraController::OnKeyboardStateUpdate(const Input::KeyboardStateChange& state)
{
    _moveInput = Vector3::Zero;

    Keyboard& keyboard = InputService::Get()->GetKeyboard();
    bool handled = false;

    if (keyboard.IsKeyPressed(KeyboardKey::W))
    {
        _moveInput += Vector3::Forward;
        handled = true;
    }

    if (keyboard.IsKeyPressed(KeyboardKey::S))
    {
        _moveInput += Vector3::Backward;
        handled = true;
    }

    if (keyboard.IsKeyPressed(KeyboardKey::D))
    {
        _moveInput += Vector3::Right;
        handled = true;
    }

    if (keyboard.IsKeyPressed(KeyboardKey::A))
    {
        _moveInput += Vector3::Left;
        handled = true;
    }

    if (keyboard.IsKeyPressed(KeyboardKey::E))
    {
        _moveInput += Vector3::Up;
        handled = true;
    }

    if (keyboard.IsKeyPressed(KeyboardKey::Q))
    {
        _moveInput += Vector3::Down;
        handled = true;
    }

    return handled;
}
