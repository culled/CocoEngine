//
// Created by cullen on 5/10/26.
//

#include "GameApplication.h"

#include "Coco/Input/InputService.h"

#include "Coco/Rendering/RenderService.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"

#include "Coco/Windowing/WindowService.h"

#include "Coco/ECS/ECSService.h"

#include "Coco/ImGui/ImGuiService.h"
#include "Coco/Rendering/RenderPasses/ClearRenderPass.h"

GameApplication::GameApplication(Engine* engine) :
    Application(engine, "Top Down Game"),
    _worldManager(),
    _renderListener(this, &GameApplication::OnRender, 0)
{
    CreateServices();
    _worldManager = CreateDefaultUnique<WorldManager>();
}

GameApplication::~GameApplication()
{
    _worldManager.reset();
}

void GameApplication::Start()
{
    _renderListener.Listen();
}

void GameApplication::Tick(const TickInfo& tickInfo)
{
    _worldManager->Tick(tickInfo);
}

void GameApplication::CreateServices()
{
    _engine->CreateService<InputService>();

    auto renderService = _engine->CreateService<RenderService>();
    GraphicsDeviceCreateParams deviceCreateParams;
    VulkanGraphicsPlatformCreateParams rendererCreateParams(*this, deviceCreateParams);
    renderService->CreatePlatform<VulkanGraphicsPlatform>(rendererCreateParams);

    auto windowService = _engine->CreateService<WindowService>();
    WindowCreateParams windowCreateParams("Top Down Game", Sizei(1280, 720));
    Ref<Window> mainWindow = windowService->CreateWindow(windowCreateParams);
    mainWindow->Show();

    _engine->CreateService<ECSService>();

    _engine->CreateService<ImGuiService>(false);
}

void GameApplication::OnRender(uint64 targetID, RenderGraph& graph, RenderScene& scene)
{
    graph.CreateRenderPassObject<ClearRenderPass>("Clear", 0, Color::Blue);

    _worldManager->Render(graph, scene);
}
