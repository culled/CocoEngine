//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_APPLICATION_H
#define COCOENGINE_APPLICATION_H

#include "Engine.h"
#include "Types/String.h"

namespace Coco
{
    /// @brief Base class for an application run by the Engine
    class Application
    {
    public:
        virtual ~Application() = default;

        /// @brief Gets the application version
        /// @return The application version
        virtual Version GetVersion() const = 0;

        /// @brief Called by the engine after all resources have been created and initialized
        virtual void Start() = 0;

        /// @brief Finishes the current tick, then causes the engine to exit with the given exit code
        /// @param exitCode The code to exit the engine with
        virtual void Quit(int exitCode = 0);

        /// @brief Gets the application's name
        /// @return The application's name
        const String& GetName() const { return _name; }

        /// @brief Gets the engine instance
        /// @return The engine instance
        Engine* GetEngine() { return _engine; }

        /// @brief Gets the engine instance
        /// @return The engine instance
        const Engine* GetEngine() const { return _engine; }

    protected:
        Application(Engine* engine, const char* name);

        Engine* _engine;
        String _name;
        TickListener _tickListener;

        /// @brief Called each tick. The default tick order for the application is 0
        /// @param tickInfo The current tick info
        virtual void Tick(const TickInfo& tickInfo) {}
    };
} // Coco

#endif //COCOENGINE_APPLICATION_H
