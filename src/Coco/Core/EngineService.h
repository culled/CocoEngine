//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_ENGINESERVICE_H
#define COCOENGINE_ENGINESERVICE_H

namespace Coco
{
    class Engine;

    /// @brief Base class for a service for the Engine
    class EngineService
    {
    public:
        virtual ~EngineService() = default;

        /// @brief Gets the engine instance
        /// @return The engine
        Engine* GetEngine() { return _engine; }

        /// @brief Gets the engine instance
        /// @return The engine
        const Engine* GetEngine() const { return _engine; }

    protected:
        Engine* _engine;

        EngineService(Engine* engine);
    };
} // Coco

#endif //COCOENGINE_ENGINESERVICE_H