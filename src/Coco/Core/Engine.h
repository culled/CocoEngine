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

    /// @brief The main engine that runs the application and holds all associated services and resources
    class Engine
    {
    public:
        static const Version EngineVersion;

        Engine(EnginePlatform* platform);
        ~Engine() noexcept;

        /// @brief Gets the static, singleton engine instance
        /// @return The singleton engine instance
        static Engine* Get() noexcept { return _singleton; }

        /// @brief Gets the static, singleton engine instance
        /// @return The singleton engine instance
        static const Engine* cGet() noexcept { return _singleton; }


        /// @brief Gets the temporary stack allocator
        /// @return The temporary stack allocator
        StackAllocator* GetTemporaryStackAllocator() noexcept { return &_temporaryStackAllocator; }

        /// @brief Creates an instance of the application
        /// @tparam AppType The application type
        /// @tparam Args Arguments to pass to the application's constructor
        /// @param args Arguments to pass to the application's constructor
        /// @return The created application
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

        /// @brief Creates an engine service
        /// @tparam ServiceType The type of service to create
        /// @tparam Args The arguments to pass to the service's constructor
        /// @param args The arguments to pass to the service's constructor
        /// @return The created service
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

        /// @brief Determines if a service has been created and exists
        /// @tparam ServiceType The type of service
        /// @return True if the type of service has been created and exists
        template<typename ServiceType>
        bool HasService() const noexcept { return _services.Contains(typeid(ServiceType)); }

        /// @brief Gets an engine service. NOTE: only use if you're sure the service exists
        /// @tparam ServiceType The type of service
        /// @return The engine service
        template<typename ServiceType>
        ServiceType* GetService()
        {
            return static_cast<ServiceType*>(_services[typeid(ServiceType)].get());
        }

        /// @brief Gets an engine service. NOTE: only use if you're sure the service exists
        /// @tparam ServiceType The type of service
        /// @return The engine service
        template<typename ServiceType>
        const ServiceType* GetService() const
        {
            return static_cast<const ServiceType*>(_services[typeid(ServiceType)].get());
        }

        /// @brief Gets an engine service, or returns nullptr if it doesn't exist
        /// @tparam ServiceType The type of service
        /// @return The engine service, or nullptr if it doesn't exist
        template<typename ServiceType>
        ServiceType* TryGetService()
        {
            if(HasService<ServiceType>())
                return GetService<ServiceType>();

            return nullptr;
        }

        /// @brief Gets an engine service, or returns nullptr if it doesn't exist
        /// @tparam ServiceType The type of service
        /// @return The engine service, or nullptr if it doesn't exist
        template<typename ServiceType>
        const ServiceType* TryGetService() const
        {
            if(HasService<ServiceType>())
                return GetService<ServiceType>();

            return nullptr;
        }

        /// @brief Gets the engine platform
        /// @return The engine platform
        EnginePlatform* GetPlatform() noexcept { return _platform; }

        /// @brief Gets the engine platform
        /// @return The engine platform
        const EnginePlatform* GetPlatform() const noexcept { return _platform; }

        /// @brief Gets the engine's logger
        /// @return The engine's logger
        Logger* GetLogger() noexcept { return _logger.get(); }

        /// @brief Gets the engine's logger
        /// @return The engine's logger
        const Logger* GetLogger() const noexcept { return _logger.get(); }

        /// @brief Gets the main process loop
        /// @return The main process loop
        ProcessLoop* GetMainLoop() noexcept { return _mainLoop.get(); }

        /// @brief Gets the main process loop
        /// @return The main process loop
        const ProcessLoop* GetMainLoop() const noexcept { return _mainLoop.get(); }

        /// @brief Gets the resource manager
        /// @return The resource manager
        ResourceManager* GetResourceManager() noexcept { return _resourceManager.get(); }

        /// @brief Gets the resource manager
        /// @return The resource manager
        const ResourceManager* GetResourceManager() const noexcept { return _resourceManager.get(); }

        /// @brief Gets the engine's filesystem
        /// @return The file system
        FileSystem* GetFileSystem() noexcept { return _fileSystem.get(); }

        /// @brief Gets the engine's filesystem
        /// @return The file system
        const FileSystem* GetFileSystem() const noexcept { return _fileSystem.get(); }

        /// @brief Gets the application
        /// @return The application
        Application* GetApplication() noexcept { return _app.get(); }

        /// @brief Gets the application
        /// @return The application
        const Application* GetApplication() const noexcept { return _app.get(); }

        /// @brief Sets the code that the engine will return once the main process loop exits
        /// @param exitCode The exit code
        void SetExitCode(int exitCode);

        /// @brief Causes the engine to immediately stop, display a message, and return the given exit code
        /// @param message The message to display
        /// @param exitCode The code to exit with
        void Crash(const char* message, int exitCode = -1);

        /// @brief Runs the application and returns the exit code
        int Run();

        /// @brief Shuts down all engine systems except for logging. This should only be called by the EnginePlatform
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
