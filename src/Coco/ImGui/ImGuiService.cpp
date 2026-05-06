//
// Created by cullen on 4/2/26.
//

#include "ImGuiService.h"

#include "Coco/Windowing/Window.h"

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/RenderService.h"
#include "Coco/Rendering/Mesh.h"
#include "Coco/Rendering/Shader.h"
#include "Coco/Windowing/WindowService.h"
#include "ImGuiRenderPass.h"

#include "Coco/Input/InputService.h"
#include "ImGuiInputLayer.h"
#include <imgui.h>

namespace Coco
{
    ImGuiViewportData::ImGuiViewportData(Ref<Window> viewportWindow) :
        ViewportWindow(viewportWindow),
        PositionChangedHandler(this, &ImGuiViewportData::OnPositionChanged),
        SuppressPositionChangedEvent(false),
        SizeChangedHandler(this, &ImGuiViewportData::OnSizeChanged),
        SuppressSizeChangedEvent(false),
        CloseRequestedHandler(this, &ImGuiViewportData::OnClosing),
        SuppressCloseRequestedEvent(false)
    {
        viewportWindow->OnPositionChanged.Connect(PositionChangedHandler);
        viewportWindow->OnSizeChanged.Connect(SizeChangedHandler);
        viewportWindow->OnCloseRequested.Connect(CloseRequestedHandler);
    }

    bool ImGuiViewportData::OnPositionChanged(const Vector2i& newPosition)
    {
        if (SuppressPositionChangedEvent)
        {
            SuppressPositionChangedEvent = false;
        }
        else if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(ViewportWindow.Get()))
        {
            viewport->PlatformRequestMove = true;
        }

        return false;
    }

    bool ImGuiViewportData::OnSizeChanged(const Sizei& newSize)
    {
        if (SuppressSizeChangedEvent)
        {
            SuppressSizeChangedEvent = false;
        }
        else if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(ViewportWindow.Get()))
        {
            viewport->PlatformRequestResize = true;
        }

        return false;
    }

    bool ImGuiViewportData::OnClosing(bool& cancel)
    {
        ImGuiIO& io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != ImGuiConfigFlags_ViewportsEnable || SuppressCloseRequestedEvent)
        {
            return false;
        }

        if (ImGuiViewport* viewport = ::ImGui::FindViewportByPlatformHandle(ViewportWindow.Get()))
        {
            // ImGui doesn't handle closing the main viewport, so we'll let the natural closing process occur
            if (viewport == ImGui::GetMainViewport())
            {
                return false;
            }

            viewport->PlatformRequestClose = true;
        }

        cancel = true;

        return true;
    }

    void ImGuiViewportData::Render(RenderGraph& graph, RenderScene& scene)
    {
        ImGuiIO& io = ::ImGui::GetIO();
        COCO_ASSERT(io.BackendPlatformUserData, "ImGui BackendPlatformUserData was null");
        ImGuiService* imGuiService = static_cast<ImGuiService*>(io.BackendPlatformUserData);

        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(ViewportWindow.Get());
        COCO_ASSERT(viewport, "Viewport couldn't be found");

        ImDrawData* drawData = viewport->DrawData;
        if (drawData->TotalVtxCount == 0)
            return;

        StackAllocator* tempAllocator = Engine::Get()->GetTemporaryStackAllocator();
        Array<Vector3> positions(tempAllocator, drawData->TotalVtxCount);
        Array<Vector4> colors(tempAllocator, drawData->TotalVtxCount);
        Array<Vector2> uvs(tempAllocator, drawData->TotalVtxCount);
        Array<uint32> indices(tempAllocator, drawData->TotalIdxCount);
        indices.Resize(drawData->TotalIdxCount);
        Array<Submesh> submeshes(tempAllocator, drawData->CmdListsCount);
        uint32 vertexOffset = 0;
        uint32 indexOffset = 0;

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* drawList = drawData->CmdLists[n];

            // Copy vertex data (position, uv, color)
            for (int v = 0; v < drawList->VtxBuffer.Size; v++)
            {
                const ImDrawVert* imVert = drawList->VtxBuffer.Data + v;

                positions.EmplaceBack(imVert->pos.x, imVert->pos.y, 0.0f);

                ImVec4 imGuiColor = ImGui::ColorConvertU32ToFloat4(imVert->col);
                Color c(imGuiColor.x, imGuiColor.y, imGuiColor.z, imGuiColor.w);
                colors.EmplaceBack(c.AsVector4(false));

                uvs.EmplaceBack(imVert->uv.x, imVert->uv.y);
            }

            // Copy index data for each submesh. Each command list will be a separate submesh
            memcpy(indices.Data() + indexOffset, drawList->IdxBuffer.Data, drawList->IdxBuffer.Size * sizeof(ImDrawIdx));
            submeshes.EmplaceBack(indexOffset, drawList->IdxBuffer.Size, vertexOffset);

            vertexOffset += drawList->VtxBuffer.Size;
            indexOffset += drawList->IdxBuffer.Size;
        }

        uint64 meshID = Math::CombineHashes(static_cast<uint64>(ViewportWindow->GetID()), typeid(ImGuiService).hash_code());
        scene.AddMeshData(meshID, positions, indices, {}, colors, {}, uvs);

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* drawList = drawData->CmdLists[n];

            for (int cmdI = 0; cmdI < drawList->CmdBuffer.Size; cmdI++)
            {
                const ImDrawCmd& cmd = drawList->CmdBuffer[cmdI];

                ImGuiObjectData objData;

                // Convert the scissor rect
                objData.ScissorRect = Recti(
                    Vector2i(
                        Math::Max(static_cast<int>(cmd.ClipRect.x - drawData->DisplayPos.x), 0),
                        Math::Max(static_cast<int>(cmd.ClipRect.y - drawData->DisplayPos.y), 0)
                    ),
                    Sizei(
                        Math::Max(static_cast<int>(cmd.ClipRect.z - drawData->DisplayPos.x), 0),
                        Math::Max(static_cast<int>(cmd.ClipRect.w - drawData->DisplayPos.y), 0)
                    )
                );

                // Get the draw texture
                objData.TextureID = static_cast<uint64>(cmd.GetTexID());

                const auto& submesh = submeshes[n];
                indexOffset = submesh.IndexOffset + cmd.IdxOffset;
                vertexOffset = submesh.VertexOffset + cmd.VtxOffset;

                uint64 drawID = Math::CombineHashes(static_cast<uint64>(cmdI), static_cast<uint64>(n), meshID);
                scene.StoreData(drawID, false, objData);
                scene.AddObject(drawID, 0, meshID, indexOffset, cmd.ElemCount, static_cast<int32>(vertexOffset));
            }
        }

        graph.CreateRenderPass<ImGuiRenderPassData>("ImGui",
            [&](RenderGraphBuilder& builder, ImGuiRenderPassData& passData)
            {
                passData.ImGuiShader = imGuiService->GetImGuiShader();
                passData.ColorOutput = builder.WriteRenderTarget(0);

                // The vertex positions are relative to the screen, so they will need to be adjusted based on the position of the viewport
                // Flip the top and bottom positions because imgui outputs with y=0 at the top of the viewport, and Coco expects y=0 to be at the bottom
                passData.Projection = scene.CreateOrthographicProjection(
                    drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x,
                    drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y, -1.0f, 1.0f
                );
            },
            [](const ImGuiRenderPassData& passData, const RenderScene& scene, RenderContext& ctx)
            {
                GraphicsPipelineState pipelineState;
                pipelineState.CullingMode = CullMode::None;
                pipelineState.BlendState = AttachmentBlendState::AlphaBlending;
                pipelineState.EnableDepthWrite = false;
                pipelineState.DepthTestMode = DepthTestingMode::Never;

                ctx.SetShader(*passData.ImGuiShader, pipelineState);

                ShaderCursor globalCursor;
                if (ctx.CreateAndBindGlobalBuffer("imguiCameraData", globalCursor))
                {
                    globalCursor.Field("Projection").Write(passData.Projection);
                }

                for (const auto& obj : scene.GetRenderObjectView())
                {
                    if (scene.HasData<ImGuiObjectData>(obj.ID, false))
                    {
                        const ImGuiObjectData* objData = scene.GetData<ImGuiObjectData>(obj.ID, false);

                        ctx.SetScissor(objData->ScissorRect);

                        SharedPtr<Texture> drawTexture = scene.GetTexture(objData->TextureID);
                        ctx.SetDrawData(nullptr, 0, Span<const SharedPtr<Texture>>({drawTexture}));

                        ctx.DrawObject(obj);
                    }
                }
            });
    }

    uint64 ImGuiService::_fontTextureResourceID = std::numeric_limits<uint64>::max();

    ImGuiService::ImGuiService(Engine* engine, bool enableViewports) :
        EngineService(engine),
        _windowService(engine->GetService<WindowService>()),
        _viewports(),
        _shader(),
        _fontTexture(),
        _inputLayer(CreateDefaultShared<ImGuiInputLayer>()),
        _newFrameTickListener(this, &ImGuiService::NewFrame, NewFrameOrder),
        _endFrameTickListener(this, &ImGuiService::EndFrame, EndFrameOrder),
        _shouldUpdateDisplays(true),
        _viewportRenderListener(this, &ImGuiService::OnRender, ImGuiRenderOrder)
    {
        if (!_windowService)
            throw Exception("No active WindowService found");

        RenderService* rendering = engine->GetService<RenderService>();
        if (!rendering)
            throw Exception("No active RenderService found");

        GraphicsPlatform* platform = rendering->GetGraphicsPlatform();
        if (!platform)
            throw Exception("No GraphicsPlatform created");

        // TODO: ImGuizmo has static allocations, which cause problems when the static Coco allocators are deleted. For now just let ImGui & ImGuizmo use the default allocators
        //ImGui::SetAllocatorFunctions(&ImGuiPlatform::ImGuiAllocCallback, &ImGuiPlatform::ImGuiFreeCallback, this);
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable docking
        io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableSetMousePos;   // TODO: set mouse cursor position

        io.BackendPlatformName = "Coco";
        io.BackendRendererName = platform->GetName();

        io.BackendPlatformUserData = this;

        if (enableViewports)
        {
            io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
            io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable viewports
        }

        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        platformIO.Platform_CreateWindow = &ImGuiService::PlatformCreateWindow;
        platformIO.Platform_DestroyWindow = &ImGuiService::PlatformDestroyWindow;
        platformIO.Platform_ShowWindow = &ImGuiService::PlatformShowWindow;
        platformIO.Platform_SetWindowPos = &ImGuiService::PlatformSetWindowPos;
        platformIO.Platform_GetWindowPos = &ImGuiService::PlatformGetWindowPos;
        platformIO.Platform_SetWindowSize = &ImGuiService::PlatformSetWindowSize;
        platformIO.Platform_GetWindowSize = &ImGuiService::PlatformGetWindowSize;
        platformIO.Platform_SetWindowFocus = &ImGuiService::PlatformFocusWindow;
        platformIO.Platform_GetWindowFocus = &ImGuiService::PlatformWindowHasFocus;
        platformIO.Platform_GetWindowMinimized = &ImGuiService::PlatformWindowMinimized;
        platformIO.Platform_SetWindowTitle = &ImGuiService::PlatformSetWindowTitle;
        platformIO.Platform_RenderWindow = nullptr;
        platformIO.Platform_SwapBuffers = nullptr;

        _newFrameTickListener.ListenTo(*engine->GetMainLoop());
        _endFrameTickListener.ListenTo(*engine->GetMainLoop());
        _viewportRenderListener.Listen();

        CreateResources();
        SetupInput();
        UpdateDisplays();
        RebuildFontTexture();

        COCO_ENGINE_LOG_VERBOSE("Created ImGuiService");
    }

    ImGuiService::~ImGuiService()
    {
        ImGuiIO& io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) == ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::DestroyPlatformWindows();
        }
        else
        {
            PlatformDestroyWindow(ImGui::GetMainViewport());
        }

        _viewports.Clear();
        io.BackendPlatformUserData = nullptr;

        ImGui::DestroyContext();

        COCO_ENGINE_LOG_VERBOSE("Destroyed ImGuiService");
    }

    void ImGuiService::PlatformCreateWindow(ImGuiViewport* viewport)
    {
        ImGuiIO& io = ::ImGui::GetIO();
        COCO_ASSERT(io.BackendPlatformUserData, "ImGui BackendPlatformUserData was null");
        ImGuiService* service = static_cast<ImGuiService*>(io.BackendPlatformUserData);

        WindowCreateParams createParams("ImGui Window", Sizei(static_cast<uint32>(viewport->Size.x), static_cast<uint32>(viewport->Size.y)));
        createParams.InitialPosition = Vector2i(static_cast<int32>(viewport->Pos.x), static_cast<int32>(viewport->Pos.y));

        if ((viewport->Flags & ImGuiViewportFlags_NoDecoration) == ImGuiViewportFlags_NoDecoration)
            createParams.StyleFlags |= WindowStyleFlags::NoDecoration;

        if ((viewport->Flags & ImGuiViewportFlags_TopMost) == ImGuiViewportFlags_TopMost)
            createParams.StyleFlags |= WindowStyleFlags::TopMost;

        if ((viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon) == ImGuiViewportFlags_NoTaskBarIcon)
            createParams.StyleFlags |= WindowStyleFlags::NoTaskbarIcon;

        if ((viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing) == ImGuiViewportFlags_NoFocusOnAppearing)
            createParams.FocusWhenShown = false;

        Ref<Window> window = service->_windowService->CreateWindow(createParams);
        service->CreateDataForViewport(viewport, window);
    }

    void ImGuiService::PlatformDestroyWindow(ImGuiViewport* viewport)
    {
        ImGuiIO& io = ::ImGui::GetIO();
        COCO_ASSERT(io.BackendPlatformUserData, "ImGui BackendPlatformUserData was null");
        ImGuiService* imGuiService = static_cast<ImGuiService*>(io.BackendPlatformUserData);

        // With viewports disabled, this is called by ImGui::DestroyContext, which requires BackendPlatformUserData to be null and thus platform will be null.
        // The viewport data is cleared upon shutdown anyways, so we don't need to worry about that case
        if (imGuiService && imGuiService->_viewports.Contains(viewport->ID))
        {
            auto& viewportData = imGuiService->_viewports.Get(viewport->ID);

            viewportData.SuppressCloseRequestedEvent = true;

            if(viewportData.ViewportWindow)
                viewportData.ViewportWindow->RequestClose();

            imGuiService->_viewports.Remove(viewport->ID);
        }

        viewport->PlatformHandle = nullptr;
        viewport->PlatformUserData = nullptr;
    }

    void ImGuiService::PlatformShowWindow(ImGuiViewport* viewport)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        viewportData->ViewportWindow->Show();
    }

    void ImGuiService::PlatformSetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        viewportData->SuppressPositionChangedEvent = true;
        viewportData->ViewportWindow->SetPosition(Vector2i(static_cast<int>(pos.x), static_cast<int>(pos.y)), true);
    }

    ImVec2 ImGuiService::PlatformGetWindowPos(ImGuiViewport* viewport)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        auto position = viewportData->ViewportWindow->GetPosition(true);

        return ImVec2(static_cast<float>(position.X()), static_cast<float>(position.Y()));
    }

    void ImGuiService::PlatformSetWindowSize(ImGuiViewport* viewport, ImVec2 size)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        viewportData->SuppressPositionChangedEvent = true;
        viewportData->ViewportWindow->SetSize(Sizei(static_cast<int>(size.x), static_cast<int>(size.y)));
    }

    ImVec2 ImGuiService::PlatformGetWindowSize(ImGuiViewport* viewport)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        Sizei size = viewportData->ViewportWindow->GetSize();

        return ImVec2(static_cast<float>(size.Width), static_cast<float>(size.Height));
    }

    void ImGuiService::PlatformFocusWindow(ImGuiViewport* viewport)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        viewportData->ViewportWindow->Focus();
    }

    bool ImGuiService::PlatformWindowHasFocus(ImGuiViewport* viewport)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        return viewportData->ViewportWindow->IsFocused();
    }

    bool ImGuiService::PlatformWindowMinimized(ImGuiViewport* viewport)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        return viewportData->ViewportWindow->GetState() == WindowState::Minimized;
    }

    void ImGuiService::PlatformSetWindowTitle(ImGuiViewport* viewport, const char* title)
    {
        COCO_ASSERT(viewport->PlatformUserData, "ImGui PlatformUserData was null");

        ImGuiViewportData* viewportData = static_cast<ImGuiViewportData*>(viewport->PlatformUserData);
        viewportData->ViewportWindow->SetTitle(title);
    }

    void ImGuiService::CreateResources()
    {
        _shader = _engine->GetResourceManager()->CreateResource<Shader>("ImGui Shader", "Shaders/BuiltIn/ImGui.slang");
    }

    void ImGuiService::SetupInput()
    {
        InputService* input = _engine->GetService<InputService>();
        input->AddInputLayer(_inputLayer);
    }

    void ImGuiService::CreateDataForViewport(ImGuiViewport* viewport, Ref<Window> window)
    {
        _viewports.Emplace(viewport->ID, window);
        viewport->PlatformHandle = static_cast<void*>(window.Get());
    }

    void ImGuiService::UpdateDisplays()
    {
        auto displays = _windowService->GetDisplays();

        ImGuiPlatformIO& platformIO = ::ImGui::GetPlatformIO();
        platformIO.Monitors.resize(static_cast<int>(displays.size()));
        for (int i = 0; i < platformIO.Monitors.size(); ++i)
        {
            const auto& display = displays[i];
            ImGuiPlatformMonitor& monitor = platformIO.Monitors[i];

            monitor.MainPos = ImVec2(static_cast<float>(display.Offset.X()), static_cast<float>(display.Offset.Y()));
            monitor.WorkPos = monitor.MainPos;
            monitor.MainSize = ImVec2(static_cast<float>(display.Resolution.Width), static_cast<float>(display.Resolution.Height));
            monitor.WorkSize = monitor.MainSize;
            monitor.DpiScale = static_cast<float>(display.DPI) / 96.0f;
        }

        _shouldUpdateDisplays = false;
    }

    void ImGuiService::RebuildFontTexture()
    {
        // Setup the font texture
        int width, height;
        unsigned char* pixels = nullptr;
        ImGuiIO& io = ::ImGui::GetIO();

        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        if (!_fontTexture)
        {
            _fontTexture = _engine->GetResourceManager()->CreateResource<Texture>(
                "ImGui Font Texture",
                ImageDescription(
                    width, height,
                    ImagePixelFormat::RGBA8, ImageColorSpace::sRGB,
                    ImageUsageFlags::Sampled,
                    false
                ),
                ImageSamplerDescription::LinearClamp);

            _fontTextureResourceID = _fontTexture->GetID();

            io.Fonts->SetTexID(
                static_cast<ImTextureID>(_fontTextureResourceID)
            );
        }
        else
        {
            _fontTexture->Resize(width, height);
        }

        uint64 pixelDataSize = static_cast<uint64>(width) * height * ImageDescription::GetChannelCount(ImagePixelFormat::RGBA8);

        _fontTexture->SetPixels(pixels, pixelDataSize);
    }

    void ImGuiService::NewFrame(const TickInfo& tickInfo)
    {
        if (_shouldUpdateDisplays)
            UpdateDisplays();

        auto mainWindow = _windowService->GetMainWindow();
        if (!mainWindow)
            return;

        ImGuiIO& io = ImGui::GetIO();

        // Update basic ImGui info
        io.DeltaTime = static_cast<float>(tickInfo.UnscaledDeltaTime.GetSeconds());

        // Update window-specific info
        Sizei mainWindowSize = mainWindow->GetSize();
        io.DisplaySize = ImVec2(static_cast<float>(mainWindowSize.Width), static_cast<float>(mainWindowSize.Height));

        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        if (!mainViewport->PlatformHandle)
            CreateDataForViewport(mainViewport, mainWindow);

        ImGui::NewFrame();
    }

    void ImGuiService::EndFrame(const TickInfo& tickInfo)
    {
        auto mainWindow = _windowService->GetMainWindow();
        if (!mainWindow)
            return;

        ImGui::EndFrame();
        ImGui::Render();

        // Update windows here so that their data will be up to date by the next frame
        const ImGuiIO& io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) == ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
        }
    }

    void ImGuiService::OnRender(uint64 targetID, RenderGraph& graph, RenderScene& scene)
    {
        for (auto& viewport : _viewports)
        {
            if (viewport.second.ViewportWindow->GetID() != targetID)
                continue;

            viewport.second.Render(graph, scene);
            break;
        }
    }
} // Coco