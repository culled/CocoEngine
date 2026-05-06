//
// Created by cullen on 2/24/26.
//

#include "Engine.h"

#include "Logging/LogSinks/StdOutLogSink.h"
#include "Application.h"
#include "IO/FileSystems/DiskFileSystem.h"

namespace Coco
{
    const Version Engine::EngineVersion = Version(0, 1, 0);
    Engine* Engine::_singleton = nullptr;

    Engine::Engine(EnginePlatform* platform) :
        _platform(platform),
        _temporaryStackAllocator(_temporaryStackGroup, _temporaryStackSize),
        _logger(CreateDefaultUnique<Logger>(LogMessageSeverity::Verbose)),
        _fileSystem(),
        _mainLoop(CreateDefaultUnique<ProcessLoop>()),
        _resourceManager(CreateDefaultUnique<ResourceManager>(this)),
        _services(),
        _app(nullptr),
        _exitCode(0)
    {
        _singleton = this;

        // TODO: temporary!
        _logger->CreateSink<StdOutLogSink>();

        // TODO: create filesystem based on startup args?
        _fileSystem = CreateDefaultUnique<DiskFileSystem>("Assets", "Cache");

        LOG_VERBOSE(_logger, "Engine initialized");
    }

    Engine::~Engine() noexcept
    {
        COCO_ASSERT(!_app, "Application was not destroyed. Did you forget to call Engine::Shutdown()?");

        LOG_VERBOSE(_logger, "Engine destroyed");
        _logger.reset();

        _singleton = nullptr;
    }

    void Engine::SetExitCode(int exitCode)
    {
        _exitCode = exitCode;
    }

    void Engine::Crash(const char* message, int exitCode)
    {
        SetExitCode(exitCode);
        LOG_CRITICAL(_logger, message);
        //_platform->ShowMessageBox("Crash", message, true);

        abort();
    }

    int Engine::Run()
    {
        if (_exitCode != 0)
            return _exitCode;

        COCO_ASSERT(_app, "Application was not created");

        try
        {
            _app->Start();
            _mainLoop->Start();
        }
        catch (std::exception& ex)
        {
            Crash(ex.what());
        }
        catch (...)
        {
            Crash("Unhandled exception during Engine::Run()");
        }

        return _exitCode;
    }

    void Engine::Shutdown()
    {
        _app.reset();

        // Destroy services in reverse order they were created
        for (int64 i = _serviceOrder.GetCount() - 1; i >= 0; --i)
            _services.Remove(_serviceOrder[i]);
        _serviceOrder.Clear(true);

        _resourceManager.reset();
        _mainLoop.reset();
        _fileSystem.reset();

        LOG_VERBOSE(_logger, "Engine shutdown");
    }
} // Coco
