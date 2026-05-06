//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_ENGINESERVICE_H
#define COCOENGINE_ENGINESERVICE_H

namespace Coco
{
    class Engine;

    class EngineService
    {
    public:
        virtual ~EngineService() = default;

        Engine* GetEngine() { return _engine; }
        const Engine* GetEngine() const { return _engine; }

    protected:
        Engine* _engine;

    protected:
        EngineService(Engine* engine);
    };
} // Coco

#endif //COCOENGINE_ENGINESERVICE_H