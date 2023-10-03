#include "RenderViewProvider3D.h"

#include <Coco/Rendering/RenderService.h>
#include <Coco/Input/InputService.h>
#include <Coco/Core/MainLoop/MainLoop.h>
#include <Coco/Core/Engine.h>
#include <Coco/Windowing/WindowService.h>

#include "RenderPass3D.h"

RenderViewProvider3D::RenderViewProvider3D(AttachmentCache& attachmentCache) :
    _clearColor(Color(0.1, 0.2, 0.3, 1.0)),
    _msaaSamples(MSAASamples::Four),
    _cameraTransform(),
    _mouseSensitivity(0.005),
    _tickListener(CreateManagedRef<TickListener>(this, &RenderViewProvider3D::Tick, 0)),
    _attachmentCache(attachmentCache)
{
    MainLoop::Get()->AddListener(_tickListener);
}

RenderViewProvider3D::~RenderViewProvider3D()
{
    MainLoop::Get()->RemoveListener(_tickListener);
}

void RenderViewProvider3D::SetupRenderView(
    RenderView& renderView,
    const CompiledRenderPipeline& pipeline,
    uint64 rendererID, 
    const SizeInt& backbufferSize, 
    std::span<Ref<Image>> backbuffers)
{
    std::vector<RenderTarget> rts = _attachmentCache.CreateRenderTargets(pipeline, rendererID, backbufferSize, _msaaSamples, backbuffers);

    for (size_t i = 0; i < rts.size(); i++)
    {
        ImagePixelFormat pixelFormat = pipeline.InputAttachments.at(i).PixelFormat;

        if (IsDepthFormat(pixelFormat) || IsStencilFormat(pixelFormat))
        {
            rts.at(i).SetDepthClearValue(1.0);
        }
        else
        {
            rts.at(i).SetColorClearValue(_clearColor);
        }
    }

    double aspectRatio = static_cast<double>(backbufferSize.Width) / backbufferSize.Height;
    RectInt viewport(Vector2Int::Zero, backbufferSize);
    Matrix4x4 view = _cameraTransform.InvGlobalTransform;
    //Matrix4x4 view = Matrix4x4::CreateLookAtMatrix(Vector3::One * 3.0, Vector3::Zero, Vector3::Up);
    //Matrix4x4 view = Matrix4x4::CreateLookAtMatrix(Vector3::Zero, Vector3::Forward, Vector3::Up);
    //Matrix4x4 projection = RenderService::Get()->GetPlatform().CreateOrthographicProjection(2.0, aspectRatio, 0.1, 100);
    Matrix4x4 projection = RenderService::Get()->GetPlatform().CreatePerspectiveProjection(Math::DegToRad(90.0), aspectRatio, 0.1, 100);
    ViewFrustum frustum = ViewFrustum::CreatePerspective(
        _cameraTransform.GetGlobalPosition(), 
        _cameraTransform.GetGlobalBackward(),
        _cameraTransform.GetGlobalUp(),
        Math::DegToRad(90.0), 
        aspectRatio, 
        0.1, 100);

    renderView.Setup(
        viewport, 
        viewport, 
        view, 
        projection, 
        _cameraTransform.GetGlobalPosition(),
        frustum,
        pipeline.SupportsMSAA ? _msaaSamples : MSAASamples::One, 
        rts);

    GlobalShaderUniformLayout globalLayout(
        {
            ShaderDataUniform("ProjectionMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
            ShaderDataUniform("ViewMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
            ShaderDataUniform("ViewPosition", ShaderStageFlags::Vertex | ShaderStageFlags::Fragment, BufferDataType::Float3),
            ShaderDataUniform("AmbientColor", ShaderStageFlags::Vertex | ShaderStageFlags::Fragment, BufferDataType::Float4)
        },
        {},
        {
            ShaderBufferUniform("Lights", ShaderStageFlags::Fragment, RenderPass3D::sLightingBufferLayout.GetTotalSize(RenderService::cGet()->GetDevice()))
        }
    );

    renderView.SetGlobalUniformLayout(globalLayout);
}

void RenderViewProvider3D::SetMSAASamples(MSAASamples samples)
{
    _msaaSamples = samples;
}

void RenderViewProvider3D::Tick(const TickInfo& tickInfo)
{
    using namespace Coco::Windowing;
    using namespace Coco::Input;

    InputService* input = InputService::Get();
    Mouse& mouse = input->GetMouse();

    Ref<Window> mainWindow = WindowService::Get()->GetMainWindow();
    if (!mouse.IsButtonPressed(MouseButton::Right))
    {
        mainWindow->SetCursorConfineMode(CursorConfineMode::None);
        mainWindow->SetCursorVisibility(true);
        return;
    }

    mainWindow->SetCursorConfineMode(CursorConfineMode::Locked);
    mainWindow->SetCursorVisibility(false);

    Vector2Int mouseDelta = mouse.GetMoveDelta();
    Vector3 eulerAngles = _cameraTransform.LocalRotation.ToEulerAngles();
    eulerAngles.X = Math::Clamp(eulerAngles.X - mouseDelta.Y * _mouseSensitivity, Math::DegToRad(-90.0), Math::DegToRad(90.0));
    eulerAngles.Y -= mouseDelta.X * _mouseSensitivity;

    _cameraTransform.LocalRotation = Quaternion(eulerAngles);

    Keyboard& keyboard = input->GetKeyboard();

    Vector3 velocity = Vector3::Zero;

    if (keyboard.IsKeyPressed(KeyboardKey::W))
        velocity += Vector3::Forward;

    if (keyboard.IsKeyPressed(KeyboardKey::S))
        velocity += Vector3::Backward;

    if (keyboard.IsKeyPressed(KeyboardKey::D))
        velocity += Vector3::Right;

    if (keyboard.IsKeyPressed(KeyboardKey::A))
        velocity += Vector3::Left;

    if (keyboard.IsKeyPressed(KeyboardKey::E))
        velocity += Vector3::Up;

    if (keyboard.IsKeyPressed(KeyboardKey::Q))
        velocity += Vector3::Down;

    velocity = _cameraTransform.LocalRotation * velocity;
    _cameraTransform.TranslateLocal(velocity * tickInfo.DeltaTime);
    _cameraTransform.Recalculate();
}
