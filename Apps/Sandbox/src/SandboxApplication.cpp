//
// Created by cullen on 2/25/26.
//

#include "SandboxApplication.h"

#include <Coco/Input/InputService.h>

#include <Coco/Windowing/WindowService.h>

#include "Coco/Core/Math/Matrix4x4.h"
#include "Coco/Core/Types/Sorting/QSorter.h"
#include "Coco/Rendering/MeshUtils.h"
#include "Coco/Rendering/RenderService.h"
#include "Coco/Rendering/RenderGraph/RenderGraph.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"

#include "Coco/Windowing/Window.h"

#include "Coco/ECS/ECSService.h"
#include "Coco/ECS/Components/Transform3DComponent.h"
#include "Coco/ECS/Components/Transform2DComponent.h"
#include "Coco/ECS/Rendering/Components/CameraComponent.h"
#include "Coco/ECS/Rendering/Components/SpriteRendererComponent.h"
#include "Coco/ECS/Rendering/Components/SpritesheetAnimationComponent.h"
#include "Coco/ImGui/ImGuiService.h"

#include <imgui.h>

#include "Coco/ECS/Rendering/Components/TileMapRendererComponent.h"
#include "Coco/Rendering/RenderPasses/ClearRenderPass.h"
#include "Coco/Rendering/RenderPasses/SimpleRenderPass.h"


SandboxApplication::SandboxApplication(Engine* engine) :
    Application(engine, "Sandbox"),
    _tickListener(this, &SandboxApplication::OnTick, 0),
    _renderListener(this, &SandboxApplication::RenderSceneCallback, 0)
{
    //engine->GetMainLoop()->SetTargetTickRate(60);
    _tickListener.ListenTo(*engine->GetMainLoop());

    CreateServices();
    CreateResources();
    CreateScene();

    COCO_ENGINE_LOG_INFO("Total memory usage: %u bytes", engine->GetPlatform()->GetMemoryManager()->GetTotalUsage());
    COCO_ENGINE_LOG_INFO("SandboxApplication created");
}

SandboxApplication::~SandboxApplication()
{
    COCO_ENGINE_LOG_INFO("SandboxApplication shutdown");
}

void SandboxApplication::Start()
{
    _window->Show();
    _renderListener.Listen();

    COCO_ENGINE_LOG_INFO("SandboxApplication started");
}

void SandboxApplication::CreateServices()
{
    _engine->CreateService<InputService>();

    RenderService* rendering = _engine->CreateService<RenderService>();
    GraphicsDeviceCreateParams deviceCreateParams;
    VulkanGraphicsPlatformCreateParams rendererCreateParams(*this, deviceCreateParams);
    #ifndef NDEBUG
    rendererCreateParams.EnableDebugging = true;
    #endif
    rendering->CreatePlatform<VulkanGraphicsPlatform>(rendererCreateParams);

    WindowService* windowing = _engine->CreateService<WindowService>();
    WindowCreateParams windowCreateParams("Sandbox", Sizei(640, 480));
    _window = windowing->CreateWindow(windowCreateParams);
    //_window->GetSurface()->SetVSyncMode(VSyncMode::Immediate);

    _engine->CreateService<ECSService>();

    _engine->CreateService<ImGuiService>(false);
}

void SandboxApplication::CreateResources()
{
    _shader = _engine->GetResourceManager()->CreateResource<Shader>("HelloTriangleShader", "Shaders/BuiltIn/Sprite.slang");

    _spriteTexture = _engine->GetResourceManager()->CreateResource<Texture>("Texture", "Textures/Cat_Anim.png", ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageSamplerDescription::NearestClamp, false);

    _texture2 = _engine->GetResourceManager()->CreateResource<Texture>("Texture2", "Textures/SpriteSheetTest.png", ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageSamplerDescription::NearestClamp, true);
    SharedPtr<TileMapAtlas> atlas = CreateDefaultShared<TileMapAtlas>(_texture2, 4, 2);
    //_texture2 = _engine->GetResourceManager()->CreateResource<Texture>("Texture2", "Textures/World of Solaria Demo Pack/Tilesets/Solaria Demo Tiles.png", ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageSamplerDescription::NearestClamp, true);
    //SharedPtr<TileMapAtlas> atlas = CreateDefaultShared<TileMapAtlas>(_texture2, 28, 15);

    _tileMap = CreateDefaultShared<TileMap>(atlas);
    for (uint32 x = 0; x < atlas->GetWidth(); x++)
    {
        for (uint32 y = 0; y < atlas->GetHeight(); y++)
        {
            _tileMap->SetCell(Vector2i(x, y), atlas->GetCellID(x, y));
        }
    }
}

void SandboxApplication::CreateScene()
{
    _scene = _engine->GetResourceManager()->CreateResource<Scene>("Scene");

    _cameraEntity = _scene->CreateEntity("Camera");
    _cameraEntity.CreateComponent<Transform2DComponent>();
    CameraComponent* camera = _cameraEntity.CreateComponent<CameraComponent>();
    camera->SetOrthographicProjection(5.0f, 0.0f, 100.0f);
    camera->ClearColor = Color(0.2f, 0.1f, 0.8f);

    _tilemapEntity = _scene->CreateEntity("Tilemap");
    _tilemapEntity.CreateComponent<Transform2DComponent>(Vector2(0.5f, 0.5f), 0.0f, Vector2::One * 0.25f);
    _tilemapEntity.CreateComponent<TileMapRendererComponent>(_tileMap);

    _spriteEntity = _scene->CreateEntity("Sprite");
    _spriteEntity.CreateComponent<Transform2DComponent>(Vector2(-1.0f, -1.0f), 0.0f, Vector2(2.0f, 1.0f));
    auto spriteComponent = _spriteEntity.CreateComponent<SpriteRendererComponent>(_spriteTexture);
    spriteComponent->SetAtlas(4, 8);
    auto spriteAnimComponent = _spriteEntity.CreateComponent<SpritesheetAnimationComponent>();
    spriteAnimComponent->AddAnimation("idle", 0, 11, 6.0);
    spriteAnimComponent->AddAnimation("run", 12, 16, 6.0);
    spriteAnimComponent->SetCurrentAnimation("idle");

    _spriteEntity2 = _scene->CreateEntity("Sprite2");
    _spriteEntity2.CreateComponent<Transform2DComponent>(Vector2(1.0f, -1.0f), 0.0f, Vector2(2.0f, 1.0f));
    spriteComponent = _spriteEntity2.CreateComponent<SpriteRendererComponent>(_spriteTexture);
    spriteComponent->SetAtlas(4, 8);
    spriteAnimComponent = _spriteEntity2.CreateComponent<SpritesheetAnimationComponent>();
    spriteAnimComponent->AddAnimation("idle", 0, 11, 6.0);
    spriteAnimComponent->AddAnimation("run", 12, 16, 6.0);
    spriteAnimComponent->SetCurrentAnimation("run");
}

void SandboxApplication::OnTick(const TickInfo& tickInfo)
{
    if (!_window)
        return;

    if (ImGui::Begin("Stats"))
    {
        RenderService* rendering = _engine->GetService<RenderService>();
        const auto& stats = rendering->GetLastFrameStats();
        ImGui::Text(
            "FPS: %u (%.2f ms)",
            static_cast<unsigned int>(1.0 / tickInfo.UnscaledDeltaTime.GetSeconds()),
            tickInfo.UnscaledDeltaTime.GetMilliseconds()
        );
        ImGui::Text("Draw Calls: %u", static_cast<unsigned int>(stats.DrawCalls));
        ImGui::Text("Triangles Drawn: %u", static_cast<unsigned int>(stats.TrianglesDrawn));
        ImGui::Text("Vertices Drawn: %u", static_cast<unsigned int>(stats.VerticesDrawn));
        ImGui::Text("Memory Usage: %u bytes", static_cast<unsigned int>(stats.MemoryUsage));
    }

    ImGui::End();

    InputService* input = _engine->GetService<InputService>();
    Keyboard* keyboard = input->GetKeyboard();
    Vector2 moveDir;

    if (keyboard->IsKeyPressed(KeyboardKey::A))
        moveDir += Vector2::Left;
    else if (keyboard->IsKeyPressed(KeyboardKey::D))
        moveDir += Vector2::Right;

    if (keyboard->IsKeyPressed(KeyboardKey::S))
        moveDir += Vector2::Down;
    else if (keyboard->IsKeyPressed(KeyboardKey::W))
        moveDir += Vector2::Up;

    Transform2DComponent* camTransform = _cameraEntity.GetComponent<Transform2DComponent>();
    camTransform->LocalPosition += moveDir * static_cast<float>(tickInfo.DeltaTime.GetSeconds());

    camTransform->RecalculateGlobalTransform();

    auto et = _spriteEntity.GetComponent<Transform2DComponent>();
    et->LocalRotation += static_cast<float>(tickInfo.DeltaTime.GetSeconds());
    et->RecalculateGlobalTransform();

    RenderService* rendering = _engine->GetService<RenderService>();
    auto gizmos = rendering->GetGizmos();
    gizmos->DrawRay3D(Vector3::Zero, Vector3::Up, Color::Green);
    gizmos->DrawRay3D(Vector3::Zero, Vector3::Right, Color::Red);
    gizmos->DrawBox3D(Vector3(-2.0f, 0.0f, 0.0f), Quaternion::Identity, Vector3::One, Color::Yellow);
    gizmos->DrawCircle3D(Vector3(-1.0f, 0.0f, 0.0f), Quaternion::Identity, 0.5f, Color::Magenta);
    gizmos->DrawSphere3D(Vector3(-1.0f, 1.5f, 0.0f), 0.75f, Color::Cyan);
    gizmos->DrawCone3D(Vector3(-2.0f, 0.5f, 0.0f), Quaternion::Identity, 0.5f, 0.25f, Color::Black);
}

struct GlobalSceneData
{
    Matrix4x4 View;
    Matrix4x4 Projection;

    void WriteInto(ShaderCursor& cursor) const
    {
        cursor.Field("View").Write(View);
        cursor.Field("Projection").Write(Projection);
    }
};

struct SpriteObjectData
{
    Matrix4x4 Model;
    Vector4 Slice;
    Vector4 TintColor;
    SharedPtr<Texture> SpriteTexture;

    void SetDrawData(RenderContext& ctx) const
    {
        uint8 data[sizeof(Matrix4x4) + sizeof(Vector4) * 2];
        memcpy(data, &Model, sizeof(Matrix4x4));
        memcpy(data + sizeof(Matrix4x4), &Slice, sizeof(Vector4));
        memcpy(data + sizeof(Matrix4x4) + sizeof(Vector4), &TintColor, sizeof(Vector4));
        ctx.SetDrawData(data, sizeof(data), Span<const SharedPtr<Texture>>({SpriteTexture}));
    }
};

void SandboxApplication::RenderSceneCallback(uint64 targetID, RenderGraph& graph, RenderScene& scene)
{
    Transform2DComponent* camTransform = _cameraEntity.GetComponent<Transform2DComponent>();
    CameraComponent* cam = _cameraEntity.GetComponent<CameraComponent>();
    scene.SetOrthographicCamera(camTransform->GetGlobalPosition(), camTransform->GetGlobalRotation(), cam->OrthographicCamera.Size, cam->OrthographicCamera.NearClip, cam->OrthographicCamera.FarClip);

    GlobalSceneData globalData;
    globalData.View = scene.GetViewMatrix();
    globalData.Projection = scene.GetProjectionMatrix();
    scene.StoreData(0, false, globalData);

    auto clearPass = graph.CreateRenderPassObject<ClearRenderPass>("Clear", 0, cam->ClearColor);

    DrawTilemap(clearPass.GetOutputResource(), graph, scene);
    DrawSprites(clearPass.GetOutputResource(), graph, scene);

    _engine->GetService<RenderService>()->GetGizmos()->Render(graph, scene);
}

void SandboxApplication::DrawTilemap(RenderGraphResourceRef colorRef, RenderGraph& graph, RenderScene& scene)
{
    Transform2DComponent* camTransform = _cameraEntity.GetComponent<Transform2DComponent>();
    CameraComponent* cam = _cameraEntity.GetComponent<CameraComponent>();

    Transform2DComponent* mapTransform = _tilemapEntity.GetComponent<Transform2DComponent>();

    // By default, the map's tiles are 1x1 unit

    // Calculate the lower left-hand point of the camera viewport
    float aspect = graph.GetAttachmentSize().GetAspectRatio<float>();
    Vector2 startingPos(camTransform->LocalPosition.X() - cam->OrthographicCamera.Size * 0.5f * aspect, camTransform->LocalPosition.Y() - cam->OrthographicCamera.Size * 0.5f);

    // Transform the camera position into the tilemap's space
    Vector2 localStartingPos = mapTransform->InverseTransformPosition(startingPos);
    Vector2i startingCell(static_cast<int>(Math::Floor(localStartingPos.X())), static_cast<int>(Math::Floor(localStartingPos.Y())));
    Vector2i cellCount(
        static_cast<int>(Math::Ceil((cam->OrthographicCamera.Size / mapTransform->LocalScale.X()) * aspect)) + 1,
        static_cast<int>(Math::Ceil(cam->OrthographicCamera.Size / mapTransform->LocalScale.Y())) + 1
    );

    TileMapRendererComponent* mapRenderer = _tilemapEntity.GetComponent<TileMapRendererComponent>();
    auto spriteMesh = SpriteRendererComponent::GetOrCreateSpriteMesh();

    for (int x = 0; x < cellCount.X(); x++)
    {
        for (int y = 0; y < cellCount.Y(); y++)
        {
            Vector2i cellCoord(x + startingCell.X(), y + startingCell.Y());
            auto cellData = mapRenderer->Map->GetCell(cellCoord);
            if (!cellData)
                continue;

            SpriteObjectData spriteData;
            spriteData.Model = mapTransform->GlobalTransform;
            spriteData.Model.M14() += (static_cast<float>(cellCoord.X()) + 0.5f) * mapTransform->LocalScale.X();
            spriteData.Model.M24() += (static_cast<float>(cellCoord.Y()) + 0.5f) * mapTransform->LocalScale.Y();

            spriteData.TintColor = Color::White.AsVector4(false);
            spriteData.Slice = mapRenderer->Map->GetAtlas()->GetCellSlice(cellData->TileID);
            spriteData.SpriteTexture = mapRenderer->Map->GetAtlas()->GetTexture();

            uint64 objectID = Math::CombineHashes(ToHash(_tilemapEntity), static_cast<uint64>(cellCoord.X()), static_cast<uint64>(cellCoord.Y()));
            scene.StoreData(objectID, true, spriteData);
            scene.AddObject(objectID, 0, *spriteMesh, 0);
        }
    }

    GraphicsPipelineState pipelineState;
    pipelineState.CullingMode = CullMode::None;
    pipelineState.BlendState = AttachmentBlendState::AlphaBlending;
    pipelineState.EnableDepthWrite = false;

    auto pass1 = graph.CreateRenderPassObject<SimpleRenderPass<GlobalSceneData, SpriteObjectData>>("Simple", colorRef, _shader, pipelineState);
}

void SandboxApplication::DrawSprites(RenderGraphResourceRef colorRef, RenderGraph& graph, RenderScene& scene)
{
    StackArray<std::pair<uint64, int>, 64> sortedObjects;
    auto spriteMesh = SpriteRendererComponent::GetOrCreateSpriteMesh();
    //const auto& eyePosition = scene.GetCameraPosition();
    const auto& currentTick = _engine->GetMainLoop()->GetCurrentTick();
    for (auto& entity : _scene->CreateComponentView<Transform2DComponent, SpriteRendererComponent, SpritesheetAnimationComponent>(true))
    {
        Transform2DComponent* transformComponent = entity.GetComponent<Transform2DComponent>();
        SpriteRendererComponent* spriteComponent = entity.GetComponent<SpriteRendererComponent>();
        SpritesheetAnimationComponent* animationComponent = entity.GetComponent<SpritesheetAnimationComponent>();

        animationComponent->Update(currentTick);
        spriteComponent->AtlasCellIndex = animationComponent->GetCurrentAnimationFrame();

        SpriteObjectData spriteData;
        spriteData.Model = transformComponent->GlobalTransform;
        spriteData.TintColor = spriteComponent->TintColor.AsVector4(false);
        spriteData.Slice = spriteComponent->GetCurrentAtlasCellSlice();
        spriteData.SpriteTexture = spriteComponent->SpriteTexture;

        uint64 objectID = ToHash(spriteComponent->OwnerID);
        scene.StoreData(objectID, true, spriteData);

        //float depth = (eyePosition - transformComponent->GetGlobalPosition()).GetLengthSquared();
        sortedObjects.EmplaceBack(objectID, transformComponent->ZIndex);
        //scene.AddObject(objectID, 0, *spriteMesh, 0);
    }

    QSorter<std::pair<uint64, int>> objectSorter(
        [](const auto& a, const auto& b)
    {
        return a.second > b.second;
    });
    objectSorter.Sort(sortedObjects);

    for (const auto& obj : sortedObjects)
        scene.AddObject(obj.first, 0, *spriteMesh, 0);

    GraphicsPipelineState pipelineState;
    pipelineState.CullingMode = CullMode::None;
    pipelineState.BlendState = AttachmentBlendState::AlphaBlending;
    pipelineState.EnableDepthWrite = false;

    auto pass1 = graph.CreateRenderPassObject<SimpleRenderPass<GlobalSceneData, SpriteObjectData>>("Sprites", colorRef, _shader, pipelineState);
}
