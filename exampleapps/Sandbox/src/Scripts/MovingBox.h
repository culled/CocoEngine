#pragma once
#include <Coco\ECS\Scripting\NativeScript.h>

using namespace Coco;

class MovingBox :
    public ECS::NativeScript
{
protected:
    void OnUpdate(const TickInfo& tickInfo) override;
};

