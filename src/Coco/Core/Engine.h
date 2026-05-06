//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_ENGINE_H
#define COCOENGINE_ENGINE_H
#include <typeindex>

#include "EnginePlatform.h"
#include "Logging/Logger.h"
#include "Memory/MemoryManager.h"
#include "Memory/Ptrs.h"
#include "ProcessLoop/ProcessLoop.h"
#include "Types/Map.h"
#include "EngineService.h"
#include "IO/FileSystem.h"

#include "Memory/Allocators/StackAllocator.h"

#include "Resources/ResourceManager.h"
#include "Types/Version.h"

namespace Coco
{
    class Application;

    class Engine
    {
    public:
        static const Version EngineVersion;

    public:
        Engine(EnginePlatform* platform);
        ~Engine() noexcept;

        static Engine* Get() noexcept { return _singleton; }
        static const Engine* cGet() noexcept { return _singleton; }

        StackAllocator* GetTemporaryStackAllocator() noexcept { return &_temporaryStackAllocator; }

        template<typename AppType, typename ... Args>
        AppType* CreateApplication(Args&& ... args)
        {
            try
            {
                _app = CreateDefaultUnique<AppType>(this, std::forward<Args>(args)...);
            }
            catch (const std::exception& ex)
            {
                Crash(ex.what());
            }
            catch (...)
            {
                Crash("Failed to create application");
            }

            return static_cast<AppType*>(_app.get());
        }

        template<typename ServiceType, typename ... Args>
        ServiceType* CreateService(Args&& ... args)
        {
            const auto& serviceTypeInfo = typeid(ServiceType);
            if (auto service = _services.TryGetValue(serviceTypeInfo))
                return static_cast<ServiceType*>(service->get());

            _serviceOrder.Append(serviceTypeInfo);
            return static_cast<ServiceType*>(
                _services.Emplace(
                    serviceTypeInfo,
                    CreateDefaultUnique<ServiceType>(this, std::forward<Args>(args)...)
                ).get()
            );
        }

        template<typename ServiceType>
        bool HasService() const noexcept { return _services.Contains(typeid(ServiceType)); }

        template<typename ServiceType>
        ServiceType* GetService()
        {
            return static_cast<ServiceType*>(_services[typeid(ServiceType)].get());
        }

        template<typename ServiceType>
        const ServiceType* GetService() const
        {
            return static_cast<const ServiceType*>(_services[typeid(ServiceType)].get());
        }

        EnginePlatform* GetPlatform() noexcept { return _platform; }
        const EnginePlatform* GetPlatform() const noexcept { return _platform; }

        Logger* GetLogger() noexcept { return _logger.get(); }
        const Logger* GetLogger() const noexcept { return _logger.get(); }

        ProcessLoop* GetMainLoop() noexcept { return _mainLoop.get(); }
        const ProcessLoop* GetMainLoop() const noexcept { return _mainLoop.get(); }

        ResourceManager* GetResourceManager() noexcept { return _resourceManager.get(); }
        const ResourceManager* GetResourceManager() const noexcept { return _resourceManager.get(); }

        FileSystem* GetFileSystem() noexcept { return _fileSystem.get(); }
        const FileSystem* GetFileSystem() const noexcept { return _fileSystem.get(); }

        Application* GetApplication() noexcept { return _app.get(); }
        const Application* GetApplication() const noexcept { return _app.get(); }

        void SetExitCode(int exitCode);
        void Crash(const char* message, int exitCode = -1);

        /// @brief Runs the application and returns the exit code
        int Run();

        /// @brief Shuts down all engine systems except for logging. Called by EnginePlatform::Shutdown()
        void Shutdown();

    private:
        static constexpr uint64 _temporaryStackSize = 1024 * 1024 * 2;
        static constexpr uint8 _temporaryStackGroup = 1;
        static Engine* _singleton;

        EnginePlatform* _platform;
        StackAllocator _temporaryStackAllocator;
        UniquePtr<Logger> _logger;
        UniquePtr<FileSystem> _fileSystem;
        UniquePtr<ProcessLoop> _mainLoop;
        UniquePtr<ResourceManager> _resourceManager;
        Map<std::type_index, UniquePtr<EngineService>> _services;
        Array<std::type_index> _serviceOrder;
        UniquePtr<Application> _app;
        int _exitCode;
    };
} // Coco

#define COCO_ENGINE_LOG_VERBOSE(Message, ...) if(auto logger = ::Coco::Engine::Get()->GetLogger()) logger->Log(Coco::LogMessageSeverity::Verbose, Message, ##__VA_ARGS__)
#define COCO_ENGINE_LOG_INFO(Message, ...) if(auto logger = ::Coco::Engine::Get()->GetLogger()) logger->Log(Coco::LogMessageSeverity::Info, Message, ##__VA_ARGS__)
#define COCO_ENGINE_LOG_WARN(Message, ...) if(auto logger = ::Coco::Engine::Get()->GetLogger()) logger->Log(Coco::LogMessageSeverity::Warning, Message, ##__VA_ARGS__)
#define COCO_ENGINE_LOG_ERROR(Message, ...) if(auto logger = ::Coco::Engine::Get()->GetLogger()) logger->Log(Coco::LogMessageSeverity::Error, Message, ##__VA_ARGS__)
#define COCO_ENGINE_LOG_CRITICAL(Message, ...) if(auto logger = ::Coco::Engine::Get()->GetLogger()) logger->Log(Coco::LogMessageSeverity::Critical, Message, ##__VA_ARGS__)

#endif //COCOENGINE_ENGINE_H
