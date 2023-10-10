#include "ImGuiCocoPlatform.h"

#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/RenderService.h>

#include <Coco/Input/InputService.h>

#include <Coco/Windowing/WindowService.h>
#include <Coco/Windowing/Window.h>

#include <Coco/Core/Engine.h>

#include <imgui.h>

namespace Coco::ImGuiCoco
{
    CocoViewportData::CocoViewportData(Windowing::Window* window) :
        Window(window),
        IgnorePositionChanged(false),
        IgnoreSizeChanged(false)
    {
        OnPositionChanged.SetCallback([&](const Vector2Int& pos) {
            ImGuiCocoPlatform::PlatformWindowPositionChangedCallback(Window);
            return false;
            });
        OnPositionChanged.Connect(window->OnPositionChanged);

        OnSizeChanged.SetCallback([&](const SizeInt& size) {
            ImGuiCocoPlatform::PlatformWindowSizeChangedCallback(Window);
            return false;
            });
        OnSizeChanged.Connect(window->OnResized);

        OnClosing.SetCallback([&](bool& cancel) {
            ImGuiCocoPlatform::PlatformWindowCloseCallback(Window);
            cancel = true;
            return true;
            });
        OnClosing.Connect(window->OnClosing);
    }

    CocoViewportData::~CocoViewportData()
    {
        OnPositionChanged.DisconnectAll();
        OnSizeChanged.DisconnectAll();
        OnClosing.DisconnectAll();
    }

    std::unordered_map<uint64, CocoViewportData> ImGuiCocoPlatform::_sViewports;
    const GlobalShaderUniformLayout ImGuiCocoPlatform::_sGlobalUniformLayout = GlobalShaderUniformLayout(
        {
            ShaderDataUniform("ProjectionMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
        },
        {},
        {}
    );

    ImGuiCocoPlatform::ImGuiCocoPlatform(bool enableViewports) :
        _renderPass(CreateSharedRef<ImGuiRenderPass>()),
        _currentlyRenderingViewport(nullptr),
        _shouldUpdateDisplays(true), 
        _viewportMeshes()
    {
        _sViewports = std::unordered_map<uint64, CocoViewportData>();

        ImGuiIO& io = ::ImGui::GetIO();
        
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable docking
        io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableSetMousePos;   // TODO: set mouse cursor position

        io.BackendPlatformName = "Coco";
        io.BackendRendererName = "Coco";

        io.BackendPlatformUserData = this;

        if (enableViewports)
        {
            io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
            io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable viewports

            InitPlatformInterface();
        }

        CreateObjects();
    }

    ImGuiCocoPlatform::~ImGuiCocoPlatform()
    {
        _renderPipeline.reset();
        _renderPass.reset();
        _shader.reset();
        _viewportMeshes.clear();
    }

    void ImGuiCocoPlatform::SetupRenderView(
        RenderView& renderView,
        const Rendering::CompiledRenderPipeline& pipeline,
        uint64 rendererID,
        const SizeInt& backbufferSize,
        std::span<Ref<Rendering::Image>> backbuffers)
    {
        using namespace Coco::Rendering;

        Assert(_currentlyRenderingViewport != nullptr)

        std::vector<RenderTarget> rts;

        for (size_t i = 0; i < backbuffers.size(); i++)
            rts.emplace_back(backbuffers[i], Vector4::Zero);

        ImDrawData* drawData = _currentlyRenderingViewport->DrawData;

        RectInt viewport(Vector2Int::Zero, SizeInt(static_cast<uint32>(drawData->DisplaySize.x), static_cast<uint32>(drawData->DisplaySize.y)));

        Matrix4x4 projection = RenderService::Get()->GetPlatform().CreateOrthographicProjection(
            viewport.GetLeft(),
            viewport.GetRight(),
            viewport.GetTop(),
            viewport.GetBottom(),
            -1.0, 1.0);

        renderView.Setup(viewport, viewport, Matrix4x4::Identity, projection, Vector3::Zero, ViewFrustum(), MSAASamples::One, rts);

        renderView.SetGlobalUniformLayout(_sGlobalUniformLayout);
    }

    void ImGuiCocoPlatform::GatherSceneData(RenderView& renderView)
    {
        using namespace Coco::Rendering;

        Assert(_currentlyRenderingViewport != nullptr)

        ImDrawData* drawData = _currentlyRenderingViewport->DrawData;

        VertexDataFormat format{};
        format.HasColor = true;
        format.HasUV0 = true;

        SharedRef<Mesh> mesh = GetOrCreateViewportMesh(_currentlyRenderingViewport);

        if (drawData->TotalVtxCount > 0)
        {
            mesh->ClearSubmeshes();

            VertexDataFormat format{};
            format.HasUV0 = true;
            format.HasColor = true;

            std::vector<VertexData> vertices(drawData->TotalVtxCount);
            std::vector<uint32> indices;
            uint64 vertexOffset = 0;

            Vector3 offset(drawData->DisplayPos.x, drawData->DisplayPos.y, 0.0);

            for (int n = 0; n < drawData->CmdListsCount; n++)
            {
                const ImDrawList* drawList = drawData->CmdLists[n];

                for (int v = 0; v < drawList->VtxBuffer.Size; v++)
                {
                    ImDrawVert* imVert = drawList->VtxBuffer.Data + v;
                    VertexData& vert = vertices.at(v + vertexOffset);

                    vert.Position = Vector3(imVert->pos.x, imVert->pos.y, 0.0) - offset;

                    ImVec4 imGuiColor = ::ImGui::ColorConvertU32ToFloat4(imVert->col);
                    Color c(imGuiColor.x, imGuiColor.y, imGuiColor.z, imGuiColor.w);
                    c.ConvertToLinear();
                    vert.Color = Vector4(c.R, c.G, c.B, c.A);

                    vert.UV0 = Vector2(imVert->uv.x, imVert->uv.y);
                }

                indices.resize(drawList->IdxBuffer.Size);
                for (int i = 0; i < drawList->IdxBuffer.Size; i++)
                {
                    indices.at(i) = *(drawList->IdxBuffer.Data + i) + static_cast<uint32>(vertexOffset);
                }

                vertexOffset += drawList->VtxBuffer.Size;
                mesh->SetIndices(indices, n);
            }

            mesh->SetVertices(format, vertices);
            mesh->Apply();

            renderView.AddMesh(*mesh);

            uint64 indexOffset = 0;

            for (int n = 0; n < drawData->CmdListsCount; n++)
            {
                const ImDrawList* drawList = drawData->CmdLists[n];

                for (int cmdI = 0; cmdI < drawList->CmdBuffer.Size; cmdI++)
                {
                    const ImDrawCmd& cmd = drawList->CmdBuffer[cmdI];

                    RectInt scissorRect(
                        Vector2Int(
                            static_cast<int>(cmd.ClipRect.x - drawData->DisplayPos.x), 
                            static_cast<int>(cmd.ClipRect.y - drawData->DisplayPos.y)),
                        Vector2Int(
                            static_cast<int>(cmd.ClipRect.z - drawData->DisplayPos.x), 
                            static_cast<int>(cmd.ClipRect.w - drawData->DisplayPos.y))
                    );

                    Texture* tex = reinterpret_cast<Texture*>(cmd.GetTexID());
                    renderView.AddRenderObject(
                        *mesh,
                        cmd.IdxOffset + indexOffset,
                        cmd.ElemCount,
                        Matrix4x4::Identity,
                        mesh->GetBounds(),
                        _shader.get(),
                        &scissorRect,
                        ShaderUniformData::ToTextureSampler(tex->GetImage(), tex->GetImageSampler())
                    );
                }

                indexOffset += drawList->IdxBuffer.Size;
            }
        }
    }

    bool ImGuiCocoPlatform::NewFrame(const TickInfo& tickInfo)
	{
		using namespace Coco::Windowing;

		WindowService* windowing = WindowService::Get();
		if (!windowing)
			throw std::exception("No active WindowService found");

		Ref<Window> mainWindow = windowing->GetMainWindow();
		if (!mainWindow.IsValid())
			return false;

		ImGuiIO& io = ::ImGui::GetIO();

        if(_shouldUpdateDisplays)
            UpdateDisplays();

		// Add basic info
		io.DeltaTime = static_cast<float>(tickInfo.UnscaledDeltaTime);

		// Add window-specific info
		SizeInt displaySize = mainWindow->GetClientAreaSize();
		io.DisplaySize = ImVec2(static_cast<float>(displaySize.Width), static_cast<float>(displaySize.Height));

        ImGuiViewport* mainViewport = ::ImGui::GetMainViewport();
        if (!mainViewport->PlatformHandle)
        {
            mainViewport->PlatformHandle = static_cast<void*>(mainWindow.Get());
        }
        
        return true;
	}

    void ImGuiCocoPlatform::RenderViewport(ImGuiViewport* viewport)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Rendering;
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);

        if (!window->IsVisible())
            return;

        RenderService* rendering = RenderService::Get();
        if (!rendering)
            throw std::exception("No active RenderService found");

        _currentlyRenderingViewport = viewport;

        std::array<SceneDataProvider*, 1> sceneProviders = { this };
        rendering->Render(window->GetPresenter(), *_renderPipeline, *this, sceneProviders);

        _currentlyRenderingViewport = nullptr;
    }

    void ImGuiCocoPlatform::PlatformCreateWindow(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        WindowService* windowing = WindowService::Get();
        if (!windowing)
            throw std::exception("No active WindowService found");

        WindowCreateParams createParams("ImGui Window", SizeInt(static_cast<uint32>(viewport->Size.x), static_cast<uint32>(viewport->Size.y)));
        createParams.InitialPosition = Vector2Int(static_cast<int32>(viewport->Pos.x), static_cast<int32>(viewport->Pos.y));

        if((viewport->Flags & ImGuiViewportFlags_NoDecoration) == ImGuiViewportFlags_NoDecoration)
            createParams.StyleFlags |= WindowStyleFlags::NoDecoration;
        
        if((viewport->Flags & ImGuiViewportFlags_TopMost) == ImGuiViewportFlags_TopMost)
            createParams.StyleFlags |= WindowStyleFlags::TopMost;
        
        if ((viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon) == ImGuiViewportFlags_NoTaskBarIcon)
            createParams.StyleFlags |= WindowStyleFlags::NoTaskbarIcon;

        Ref<Window> window = windowing->CreateWindow(createParams);
        CocoViewportData& viewportData = _sViewports.try_emplace(window->ID, window.Get()).first->second;

        viewport->PlatformHandle = static_cast<void*>(window.Get());
        viewport->PlatformUserData = &viewportData;
    }

    void ImGuiCocoPlatform::PlatformDestroyWindow(ImGuiViewport* viewport)
    {
        if (viewport->PlatformHandle)
        {
            using namespace Coco::Windowing;

            Window* window = static_cast<Window*>(viewport->PlatformHandle);

            auto it = _sViewports.find(window->ID);
            if (it != _sViewports.end())
            {
                _sViewports.erase(it);

                window->Close();

                viewport->PlatformHandle = nullptr;

                ImGuiCocoPlatform* platform = ImGuiCocoPlatform::Get();

                platform->RemoveViewportMesh(viewport);
            }
        }

        viewport->PlatformUserData = nullptr;
    }

    void ImGuiCocoPlatform::PlatformShowWindow(ImGuiViewport* viewport)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        window->Show();
    }

    void ImGuiCocoPlatform::PlatformSetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        CocoViewportData* vd = static_cast<CocoViewportData*>(viewport->PlatformUserData);
        vd->IgnorePositionChanged = true;
        vd->Window->SetPosition(Vector2Int(static_cast<int>(pos.x), static_cast<int>(pos.y)), true, false);
    }

    ImVec2 ImGuiCocoPlatform::PlatformGetWindowPos(ImGuiViewport* viewport)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        Vector2Int pos = window->GetPosition(true, false);
        return ImVec2(static_cast<float>(pos.X), static_cast<float>(pos.Y));
    }

    void ImGuiCocoPlatform::PlatformSetWindowSize(ImGuiViewport* viewport, ImVec2 size)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        CocoViewportData* vd = static_cast<CocoViewportData*>(viewport->PlatformUserData);
        vd->IgnoreSizeChanged = true;
        vd->Window->SetClientAreaSize(SizeInt(static_cast<uint32>(size.x), static_cast<uint32>(size.y)));
    }

    ImVec2 ImGuiCocoPlatform::PlatformGetWindowSize(ImGuiViewport* viewport)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        SizeInt size = window->GetClientAreaSize();
        return ImVec2(static_cast<float>(size.Width), static_cast<float>(size.Height));
    }

    void ImGuiCocoPlatform::PlatformFocusWindow(ImGuiViewport* viewport)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        window->Focus();
    }

    bool ImGuiCocoPlatform::PlatformWindowHasFocus(ImGuiViewport* viewport)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        return window->HasFocus();
    }

    bool ImGuiCocoPlatform::PlatformWindowMinimized(ImGuiViewport* viewport)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        return window->GetState() == WindowState::Minimized;
    }

    void ImGuiCocoPlatform::PlatformSetWindowTitle(ImGuiViewport* viewport, const char* title)
    {
        Assert(viewport->PlatformHandle)

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        window->SetTitle(title);
    }

    void ImGuiCocoPlatform::PlatformWindowPositionChangedCallback(Windowing::Window* window)
    {
        if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        {
            CocoViewportData* vd = static_cast<CocoViewportData*>(viewport->PlatformUserData);
            if(!vd->IgnorePositionChanged)
                viewport->PlatformRequestMove = true;

            vd->IgnorePositionChanged = false;
        }
    }

    void ImGuiCocoPlatform::PlatformWindowSizeChangedCallback(Windowing::Window* window)
    {
        if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        {
            CocoViewportData* vd = static_cast<CocoViewportData*>(viewport->PlatformUserData);
            if (!vd->IgnoreSizeChanged)
                viewport->PlatformRequestResize = true;

            vd->IgnoreSizeChanged = false;
        }
    }

    void ImGuiCocoPlatform::PlatformWindowCloseCallback(Windowing::Window* window)
    {
        if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        {
            viewport->PlatformRequestClose = true;
        }
    }

    void ImGuiCocoPlatform::InitPlatformInterface()
    {
        ImGuiPlatformIO& platformIO = ::ImGui::GetPlatformIO();
        platformIO.Platform_CreateWindow = &ImGuiCocoPlatform::PlatformCreateWindow;
        platformIO.Platform_DestroyWindow = &ImGuiCocoPlatform::PlatformDestroyWindow;
        platformIO.Platform_ShowWindow = &ImGuiCocoPlatform::PlatformShowWindow;
        platformIO.Platform_SetWindowPos = &ImGuiCocoPlatform::PlatformSetWindowPos;
        platformIO.Platform_GetWindowPos = &ImGuiCocoPlatform::PlatformGetWindowPos;
        platformIO.Platform_SetWindowSize = &ImGuiCocoPlatform::PlatformSetWindowSize;
        platformIO.Platform_GetWindowSize = &ImGuiCocoPlatform::PlatformGetWindowSize;
        platformIO.Platform_SetWindowFocus = &ImGuiCocoPlatform::PlatformFocusWindow;
        platformIO.Platform_GetWindowFocus = &ImGuiCocoPlatform::PlatformWindowHasFocus;
        platformIO.Platform_GetWindowMinimized = &ImGuiCocoPlatform::PlatformWindowMinimized;
        platformIO.Platform_SetWindowTitle = &ImGuiCocoPlatform::PlatformSetWindowTitle;
        platformIO.Platform_RenderWindow = nullptr;
        platformIO.Platform_SwapBuffers = nullptr;
    }

    void ImGuiCocoPlatform::CreateObjects()
    {
        // Create the pipeline
        _renderPipeline = CreateUniqueRef<Rendering::RenderPipeline>();
        std::array<uint8, 1> bindings = { 0 };
        _renderPipeline->AddRenderPass(_renderPass, bindings);

        // Setup ImGui shader
        GraphicsPipelineState pipelineState{};
        pipelineState.WindingMode = TriangleWindingMode::CounterClockwise;
        pipelineState.CullingMode = CullMode::None;
        pipelineState.DepthTestingMode = DepthTestingMode::Never;
        pipelineState.EnableDepthWrite = false;

        ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();
        _shader = resources.Create<Shader>("ImGui", ImGuiRenderPass::sPassName);
        _shader->AddVariant(
            ShaderVariant(
                ImGuiRenderPass::sPassName,
                {
                    ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/ImGui.vert.spv"),
                    ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/ImGui.frag.spv")
                },
                pipelineState,
                {
                    BlendState::AlphaBlending
                },
                {
                    ShaderVertexAttribute("Position", BufferDataType::Float3),
                    ShaderVertexAttribute("Color", BufferDataType::Float4),
                    ShaderVertexAttribute("UV", BufferDataType::Float2)
                },
                GlobalShaderUniformLayout(),
                ShaderUniformLayout(),
                ShaderUniformLayout(
                    {},
                    {
                        ShaderTextureUniform("Texture", ShaderStageFlags::Fragment, ShaderTextureUniform::DefaultTextureType::Checker)
                    }
                )
            )
        );

        // Setup the font texture
        int width, height;
        unsigned char* pixels = nullptr;
        ImGuiIO& io = ::ImGui::GetIO();

        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        _texture = resources.Create<Texture>(
            "ImGui",
            ImageDescription(
                static_cast<uint32>(width), static_cast<uint32>(height),
                ImagePixelFormat::RGBA8, ImageColorSpace::sRGB,
                ImageUsageFlags::Sampled | ImageUsageFlags::TransferDestination,
                false
            ),
            ImageSamplerDescription::LinearClamp);

        _texture->SetPixels(0, pixels, static_cast<uint64>(width) * height * GetPixelFormatChannelCount(ImagePixelFormat::RGBA8));

        io.Fonts->SetTexID(_texture.get());
    }

    void ImGuiCocoPlatform::UpdateDisplays()
    {
        using namespace Coco::Windowing;

        WindowService* windowing = WindowService::Get();
        if (!windowing)
            throw std::exception("No active WindowService found");

        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

        _displays = windowing->GetDisplays();

        platformIO.Monitors.resize(0);
        for (int i = 0; i < _displays.size(); i++)
        {
            DisplayInfo& display = _displays.at(i);

            ImGuiPlatformMonitor monitor{};
            monitor.MainPos = ImVec2(static_cast<float>(display.Offset.X), static_cast<float>(display.Offset.Y));
            monitor.WorkPos = monitor.MainPos;
            monitor.MainSize = ImVec2(static_cast<float>(display.Resolution.Width), static_cast<float>(display.Resolution.Height));
            monitor.WorkSize = monitor.MainSize;
            monitor.DpiScale = static_cast<float>(display.DPI) / Window::DefaultDPI;
            monitor.PlatformHandle = static_cast<void*>(&display);
            platformIO.Monitors.push_back(monitor);
        }

        _shouldUpdateDisplays = false;
    }

    uint64 ImGuiCocoPlatform::GetViewportKey(ImGuiViewport* viewport)
    {
        const std::hash<ImGuiViewport*> hasher;
        return hasher(viewport);
    }

    SharedRef<Mesh> ImGuiCocoPlatform::GetOrCreateViewportMesh(ImGuiViewport* viewport)
    {
        uint64 key = GetViewportKey(viewport);

        if (!_viewportMeshes.contains(key))
        {
            _viewportMeshes.try_emplace(key, Engine::Get()->GetResourceLibrary().Create<Mesh>("ImGui", true));
        }

        return _viewportMeshes.at(key);
    }

    void ImGuiCocoPlatform::RemoveViewportMesh(ImGuiViewport* viewport)
    {
        uint64 key = GetViewportKey(viewport);

        if (_viewportMeshes.contains(key))
            _viewportMeshes.erase(key);
    }
}