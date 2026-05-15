//
// Created by cullen on 2/24/26.
//

#include <Coco/Core/Engine.h>
#include <Coco/Platforms/Linux/LinuxEnginePlatform.h>

#include "GameApplication.h"

using namespace Coco;

int main()
{
    LinuxEnginePlatform platform;
    auto* engine = platform.CreateEngine();
    engine->CreateApplication<GameApplication>();

    int result = engine->Run();

    platform.Shutdown();

    return result;
}
