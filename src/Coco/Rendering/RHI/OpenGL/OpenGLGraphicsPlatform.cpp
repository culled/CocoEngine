//
// Created by cullen on 3/4/26.
//

#include "OpenGLGraphicsPlatform.h"

#include "Coco/Core/Engine.h"
#include "Resources/OpenGLGraphicsSurface.h"
#include "Resources/OpenGLImage.h"
#include "Resources/OpenGLRenderContext.h"

#include "Resources/OpenGLBuffer.h"
#include "Resources/OpenGLShaderProgram.h"
#include "OpenGLRenderFrame.h"
#include "OpenGLShaderProgramCompiler.h"
#include <glad/glad.h>

namespace Coco
{
    void OpenGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                COCO_ENGINE_LOG_CRITICAL(message);
                return;
            case GL_DEBUG_SEVERITY_MEDIUM:
                COCO_ENGINE_LOG_ERROR(message);
                return;
            case GL_DEBUG_SEVERITY_LOW:
                COCO_ENGINE_LOG_WARN(message);
                return;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
            default:
                COCO_ENGINE_LOG_VERBOSE(message);
                return;
        }
    }

    OpenGLGraphicsPlatform::OpenGLGraphicsPlatform(RenderService* renderService) :
        GraphicsPlatform(renderService),
        _display(nullptr),
        _initialized(false),
        _renderFrames(nullptr, 2),
        _currentRenderFrameIndex(0),
        _currentFrameNumber(0),
        _resources(),
        _nextResourceID(0),
        _meshStorage(),
        _shaderProgramCompiler()
    {
        _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        if (!_display)
            throw Exception("No valid EGL display found");

        EGLint major, minor;
        if (!eglInitialize(_display, &major, &minor))
            throw Exception("eglInitialize failed");

        COCO_ENGINE_LOG_INFO("Initialized EGL - Version %d.%d", major, minor);

        // Use OpenGL
        eglBindAPI(EGL_OPENGL_API);

        for (uint64 i = 0; i < _renderFrames.GetCapacity(); i++)
            _renderFrames.EmplaceBack(CreateDefaultManagedRef<OpenGLRenderFrame>(this));

        _meshStorage = CreateDefaultUnique<OpenGLMeshStorage>(this);

        _shaderProgramCompiler = CreateDefaultUnique<OpenGLShaderProgramCompiler>();

        COCO_ENGINE_LOG_VERBOSE("Created OpenGLRenderPlatform");
    }

    OpenGLGraphicsPlatform::~OpenGLGraphicsPlatform()
    {
        _shaderProgramCompiler.reset();
        _meshStorage.reset();
        _renderFrames.Clear(true);
        _resources.Clear();

        eglTerminate(_display);

        COCO_ENGINE_LOG_VERBOSE("Destroyed OpenGLRenderPlatform");
    }

    void OpenGLGraphicsPlatform::EnsureInitialized()
    {
        if (_initialized)
            return;

        int version = gladLoadGL();
        if (version == 0)
            throw Exception("glad loading failed");

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        _platformVersion = Version(major, minor, 0);
        COCO_ENGINE_LOG_INFO("Initialized OpenGL %s on %s", ToString(_platformVersion).CStr(), glGetString(GL_VENDOR));

        _initialized = true;
    }

    Ref<RenderFrame> OpenGLGraphicsPlatform::GetCurrentRenderFrame()
    {
        return Ref<OpenGLRenderFrame>(_renderFrames[_currentRenderFrameIndex]);
    }

    void OpenGLGraphicsPlatform::EndCurrentRenderFrame()
    {
        _renderFrames[_currentRenderFrameIndex]->EndFrame();
        _currentRenderFrameIndex = (_currentRenderFrameIndex + 1) % _renderFrames.GetCount();
        _renderFrames[_currentRenderFrameIndex]->NewFrame();
        _meshStorage->ClearFrameMeshBuffers(_currentRenderFrameIndex);
        ++_currentFrameNumber;
    }

    Ref<Image> OpenGLGraphicsPlatform::CreateImage(const ImageDescription& imageDescription)
    {
        uint64 id = _nextResourceID++;
        Ref<GraphicsResource> res = _resources.Emplace(id, CreateDefaultManagedRef<OpenGLImage>(this, id, imageDescription));
        return res.Downcast<Image>();
    }

    Ref<RenderContext> OpenGLGraphicsPlatform::CreateRenderContext()
    {
        uint64 id = _nextResourceID++;
        Ref<GraphicsResource> res = _resources.Emplace(id, CreateDefaultManagedRef<OpenGLRenderContext>(this, id));
        return res.Downcast<RenderContext>();
    }

    Ref<ShaderProgram> OpenGLGraphicsPlatform::CreateShaderProgram(const FilePath& shaderPath)
    {
        uint64 id = _nextResourceID++;
        Ref<GraphicsResource> res = _resources.Emplace(id, CreateDefaultManagedRef<OpenGLShaderProgram>(this, id, shaderPath));
        return res.Downcast<ShaderProgram>();
    }

    Ref<Buffer> OpenGLGraphicsPlatform::CreateBuffer(uint64 size, BufferUsageFlags usageFlags)
    {
        uint64 id = _nextResourceID++;
        Ref<GraphicsResource> res = _resources.Emplace(id, CreateDefaultManagedRef<OpenGLBuffer>(this, id, size, usageFlags));
        return res.Downcast<OpenGLBuffer>();
    }

    void OpenGLGraphicsPlatform::InvalidateResource(uint64 resourceID)
    {
        auto managedResource = _resources.TryGetValue(resourceID);
        if (managedResource && managedResource->GetUseCount() <= 1)
            _resources.Remove(resourceID);
    }

    Ref<OpenGLGraphicsSurface> OpenGLGraphicsPlatform::CreateSurface(EGLConfig config, EGLSurface surface, EGLContext context, const Sizei& framebufferSize)
    {
        uint64 id = _nextResourceID++;
        Ref<GraphicsResource> res = _resources.Emplace(id, CreateDefaultManagedRef<OpenGLGraphicsSurface>(this, id, config, surface, context, framebufferSize));
        return res.Downcast<OpenGLGraphicsSurface>();
    }
} // Coco