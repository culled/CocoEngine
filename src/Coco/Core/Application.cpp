//
// Created by cullen on 2/24/26.
//

#include "Application.h"

namespace Coco
{
    void Application::Quit(int exitCode)
    {
        _engine->SetExitCode(exitCode);
        _engine->GetMainLoop()->Stop();
    }

    Application::Application(Engine* engine, const char* name) :
        _engine(engine),
        _name(name)
    {}
} // Coco
