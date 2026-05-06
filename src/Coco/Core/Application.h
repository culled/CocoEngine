//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_APPLICATION_H
#define COCOENGINE_APPLICATION_H

#include "Engine.h"
#include "Types/String.h"

namespace Coco
{
    class Application
    {
    public:
        virtual ~Application() = default;

        virtual Version GetVersion() const = 0;
        virtual void Start() = 0;
        virtual void Quit(int exitCode = 0);

        const String& GetName() const { return _name; }
        Engine* GetEngine() { return _engine; }
        const Engine* GetEngine() const { return _engine; }

    protected:
        Application(Engine* engine, const char* name);

    protected:
        Engine* _engine;
        String _name;
    };
} // Coco

#endif //COCOENGINE_APPLICATION_H
