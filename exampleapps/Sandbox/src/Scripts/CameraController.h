#pragma once

#include <Coco/ECS/Scripting/NativeScript.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Input/UnhandledInputLayer.h>

using namespace Coco;

class CameraController :
    public Coco::ECS::NativeScript
{
private:
    double _mouseSensitivity;
    Vector3 _moveInput;
    bool _isLooking;

    EventHandler<const Input::MouseStateChange&> _mouseStateChangedHandler;
    EventHandler<const Input::KeyboardStateChange&> _keyboardStateChangedHandler;

public:
    CameraController();

protected:
    void OnCreate() override;
    void OnStart() override;
    void OnUpdate(const TickInfo& tickInfo) override;
    void OnDestroy() override;

private:
    bool OnMouseStateUpdate(const Input::MouseStateChange& state);
    bool OnKeyboardStateUpdate(const Input::KeyboardStateChange& state);
};

