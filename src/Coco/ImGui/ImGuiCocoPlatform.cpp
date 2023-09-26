#include "ImGuiCocoPlatform.h"

#include <Coco/Input/InputService.h>
#include <Coco/Windowing/WindowService.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>
#include <imgui.h>
#include "ImGuiRenderPass.h"

namespace Coco::ImGuiCoco
{
	ImGuiKey ToImGuiKey(Input::KeyboardKey key)
	{
		using namespace Coco::Input;

		switch (key)
		{
		case KeyboardKey::Backspace:
			return ImGuiKey_Backspace;
		case KeyboardKey::Tab:
			return ImGuiKey_Tab;

        case KeyboardKey::Enter:
            return ImGuiKey_Enter;

        case KeyboardKey::Pause:
            return ImGuiKey_Pause;
        case KeyboardKey::CapsLock:
            return ImGuiKey_CapsLock;

        case KeyboardKey::Escape:
            return ImGuiKey_Escape;

        case KeyboardKey::Space:
            return ImGuiKey_Space;
        case KeyboardKey::PageUp:
            return ImGuiKey_PageUp;
        case KeyboardKey::PageDown:
            return ImGuiKey_PageDown;
        case KeyboardKey::End:
            return ImGuiKey_End;
        case KeyboardKey::Home:
            return ImGuiKey_Home;
        case KeyboardKey::ArrowLeft:
            return ImGuiKey_LeftArrow;
        case KeyboardKey::ArrowUp:
            return ImGuiKey_UpArrow;
        case KeyboardKey::ArrowRight:
            return ImGuiKey_RightArrow;
        case KeyboardKey::ArrowDown:
            return ImGuiKey_DownArrow;

        case KeyboardKey::PrintScreen:
            return ImGuiKey_PrintScreen;
        case KeyboardKey::Insert:
            return ImGuiKey_Insert;
        case KeyboardKey::Delete:
            return ImGuiKey_Delete;

        case KeyboardKey::D0:
            return ImGuiKey_0;
        case KeyboardKey::D1:
            return ImGuiKey_1;
        case KeyboardKey::D2:
            return ImGuiKey_2;
        case KeyboardKey::D3:
            return ImGuiKey_3;
        case KeyboardKey::D4:
            return ImGuiKey_4;
        case KeyboardKey::D5:
            return ImGuiKey_5;
        case KeyboardKey::D6:
            return ImGuiKey_6;
        case KeyboardKey::D7:
            return ImGuiKey_7;
        case KeyboardKey::D8:
            return ImGuiKey_8;
        case KeyboardKey::D9:
            return ImGuiKey_9;

        case KeyboardKey::A:
            return ImGuiKey_A;
        case KeyboardKey::B:
            return ImGuiKey_B;
        case KeyboardKey::C:
            return ImGuiKey_C;
        case KeyboardKey::D:
            return ImGuiKey_D;
        case KeyboardKey::E:
            return ImGuiKey_E;
        case KeyboardKey::F:
            return ImGuiKey_F;
        case KeyboardKey::G:
            return ImGuiKey_G;
        case KeyboardKey::H:
            return ImGuiKey_H;
        case KeyboardKey::I:
            return ImGuiKey_I;
        case KeyboardKey::J:
            return ImGuiKey_J;
        case KeyboardKey::K:
            return ImGuiKey_K;
        case KeyboardKey::L:
            return ImGuiKey_L;
        case KeyboardKey::M:
            return ImGuiKey_M;
        case KeyboardKey::N:
            return ImGuiKey_N;
        case KeyboardKey::O:
            return ImGuiKey_O;
        case KeyboardKey::P:
            return ImGuiKey_P;
        case KeyboardKey::Q:
            return ImGuiKey_Q;
        case KeyboardKey::R:
            return ImGuiKey_R;
        case KeyboardKey::S:
            return ImGuiKey_S;
        case KeyboardKey::T:
            return ImGuiKey_T;
        case KeyboardKey::U:
            return ImGuiKey_U;
        case KeyboardKey::V:
            return ImGuiKey_V;
        case KeyboardKey::W:
            return ImGuiKey_W;
        case KeyboardKey::X:
            return ImGuiKey_X;
        case KeyboardKey::Y:
            return ImGuiKey_Y;
        case KeyboardKey::Z:
            return ImGuiKey_Z;

        case KeyboardKey::LeftMeta:
            return ImGuiKey_LeftSuper;
        case KeyboardKey::RightMeta:
            return ImGuiKey_RightSuper;

        case KeyboardKey::Numpad0:
            return ImGuiKey_Keypad0;
        case KeyboardKey::Numpad1:
            return ImGuiKey_Keypad1;
        case KeyboardKey::Numpad2:
            return ImGuiKey_Keypad2;
        case KeyboardKey::Numpad3:
            return ImGuiKey_Keypad3;
        case KeyboardKey::Numpad4:
            return ImGuiKey_Keypad4;
        case KeyboardKey::Numpad5:
            return ImGuiKey_Keypad5;
        case KeyboardKey::Numpad6:
            return ImGuiKey_Keypad6;
        case KeyboardKey::Numpad7:
            return ImGuiKey_Keypad7;
        case KeyboardKey::Numpad8:
            return ImGuiKey_Keypad8;
        case KeyboardKey::Numpad9:
            return ImGuiKey_Keypad9;

        case KeyboardKey::NumpadMultiply:
            return ImGuiKey_KeypadMultiply;
        case KeyboardKey::NumpadPlus:
            return ImGuiKey_KeypadAdd;
        case KeyboardKey::NumpadMinus:
            return ImGuiKey_KeypadSubtract;
        case KeyboardKey::NumpadDecimal:
            return ImGuiKey_KeypadDecimal;
        case KeyboardKey::NumpadDivide:
            return ImGuiKey_KeypadDivide;

        case KeyboardKey::F1:
            return ImGuiKey_F1;
        case KeyboardKey::F2:
            return ImGuiKey_F2;
        case KeyboardKey::F3:
            return ImGuiKey_F3;
        case KeyboardKey::F4:
            return ImGuiKey_F4;
        case KeyboardKey::F5:
            return ImGuiKey_F5;
        case KeyboardKey::F6:
            return ImGuiKey_F6;
        case KeyboardKey::F7:
            return ImGuiKey_F7;
        case KeyboardKey::F8:
            return ImGuiKey_F8;
        case KeyboardKey::F9:
            return ImGuiKey_F9;
        case KeyboardKey::F10:
            return ImGuiKey_F10;
        case KeyboardKey::F11:
            return ImGuiKey_F11;
        case KeyboardKey::F12:
            return ImGuiKey_F12;

        case KeyboardKey::NumLock:
            return ImGuiKey_NumLock;
        case KeyboardKey::ScrollLock:
            return ImGuiKey_ScrollLock;

        case KeyboardKey::LeftShift:
            return ImGuiKey_LeftShift;
        case KeyboardKey::RightShift:
            return ImGuiKey_RightShift;
        case KeyboardKey::LeftControl:
            return ImGuiKey_LeftCtrl;
        case KeyboardKey::RightControl:
            return ImGuiKey_RightCtrl;
        case KeyboardKey::LeftAlt:
            return ImGuiKey_LeftAlt;
        case KeyboardKey::RightAlt:
            return ImGuiKey_RightAlt;

        case KeyboardKey::Semicolon:
            return ImGuiKey_Semicolon;

        case KeyboardKey::Equals:
            return ImGuiKey_Equal;
        case KeyboardKey::Comma:
            return ImGuiKey_Comma;
        case KeyboardKey::Minus:
            return ImGuiKey_Minus;
        case KeyboardKey::Period:
            return ImGuiKey_Period;

        case KeyboardKey::ForwardSlash:
            return ImGuiKey_Slash;

        case KeyboardKey::BackQuote:
            return ImGuiKey_GraveAccent;

        case KeyboardKey::LeftBracket:
            return ImGuiKey_LeftBracket;

        case KeyboardKey::BackSlash:
            return ImGuiKey_Backslash;

        case KeyboardKey::RightBracket:
            return ImGuiKey_RightBracket;

        case KeyboardKey::SingleQuote:
            return ImGuiKey_Apostrophe;

        default:
            return ImGuiKey_None;
		}
	}

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
        OnPositionChanged.Disconnect();
        OnSizeChanged.Disconnect();
        OnClosing.Disconnect();
    }

    std::unordered_map<uint64, CocoViewportData> ImGuiCocoPlatform::_sViewports;

    ImGuiCocoPlatform::ImGuiCocoPlatform() :
        _renderPass(CreateSharedRef<ImGuiRenderPass>()),
        _currentlyRenderingViewport(nullptr)
    {
        _sViewports = std::unordered_map<uint64, CocoViewportData>();

        ImGuiIO& io = ::ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableSetMousePos; // TODO: set mouse cursor position
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable viewports
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking

        io.BackendPlatformName = "Coco";
        io.BackendRendererName = "Coco";

        io.BackendPlatformUserData = this;

        InitPlatformInterface();
        CreateObjects();
        UpdateDisplays();
    }

    ImGuiCocoPlatform::~ImGuiCocoPlatform()
    {
        ::ImGui::DestroyPlatformWindows();
        _sViewports.clear();

        _renderPipeline.reset();
        _renderPass.reset();
        _shader.Invalidate();
        _mesh.Invalidate();
        _material.Invalidate();
    }

    UniqueRef<Rendering::RenderView> ImGuiCocoPlatform::CreateRenderView(
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

        return CreateUniqueRef<RenderView>(viewport, viewport, Matrix4x4::Identity, projection, MSAASamples::One, rts);
    }

    void ImGuiCocoPlatform::GatherSceneData(RenderView& renderView)
    {
        using namespace Coco::Rendering;

        Assert(_currentlyRenderingViewport != nullptr)

        ImDrawData* drawData = _currentlyRenderingViewport->DrawData;

        VertexDataFormat format{};
        format.HasColor = true;
        format.HasUV0 = true;

        if (drawData->TotalVtxCount > 0)
        {
            _mesh->ClearSubmeshes();

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
                _mesh->SetIndices(indices, n);
            }

            _mesh->SetVertices(format, vertices);
            _mesh->Apply();

            renderView.AddMesh(*_mesh);

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

                    renderView.AddRenderObject(
                        *_mesh,
                        cmd.IdxOffset + indexOffset,
                        cmd.ElemCount,
                        Matrix4x4::Identity,
                        _material.Get(),
                        &scissorRect
                    );
                }

                indexOffset += drawList->IdxBuffer.Size;
            }
        }
    }

    bool ImGuiCocoPlatform::NewFrame(const TickInfo& tickInfo)
	{
		using namespace Coco::Windowing;
		using namespace Coco::Input;

		WindowService* windowing = WindowService::Get();
		if (!windowing)
			throw std::exception("No active WindowService found");

		InputService* input = InputService::Get();
		if (!input)
			throw std::exception("No active InputService found");

		Ref<Window> mainWindow = windowing->GetMainWindow();
		if (!mainWindow.IsValid())
			return false;

		ImGuiIO& io = ::ImGui::GetIO();

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

		// Add mouse events
		Mouse& mouse = input->GetMouse();
		if (!io.WantSetMousePos)
		{
			Vector2Int mousePos = mouse.GetPosition();
			io.AddMousePosEvent(static_cast<float>(mousePos.X), static_cast<float>(mousePos.Y));
		}

		io.AddMouseButtonEvent(0, mouse.IsButtonPressed(MouseButton::Left));
		io.AddMouseButtonEvent(1, mouse.IsButtonPressed(MouseButton::Right));
		io.AddMouseButtonEvent(2, mouse.IsButtonPressed(MouseButton::Middle));
		io.AddMouseButtonEvent(3, mouse.IsButtonPressed(MouseButton::Button3));
		io.AddMouseButtonEvent(4, mouse.IsButtonPressed(MouseButton::Button4));

		Vector2Int scrollDelta = mouse.GetScrollWheelDelta();
		if (scrollDelta != Vector2Int::Zero)
			io.AddMouseWheelEvent(static_cast<float>(scrollDelta.X), static_cast<float>(scrollDelta.Y));

		Keyboard& keyboard = input->GetKeyboard();
        io.AddKeyEvent(ImGuiMod_Ctrl, keyboard.IsKeyPressed(KeyboardKey::LeftControl) || keyboard.IsKeyPressed(KeyboardKey::RightControl));
        io.AddKeyEvent(ImGuiMod_Shift, keyboard.IsKeyPressed(KeyboardKey::LeftShift) || keyboard.IsKeyPressed(KeyboardKey::RightShift));
        io.AddKeyEvent(ImGuiMod_Alt, keyboard.IsKeyPressed(KeyboardKey::LeftAlt) || keyboard.IsKeyPressed(KeyboardKey::RightAlt));
        io.AddKeyEvent(ImGuiMod_Super, keyboard.IsKeyPressed(KeyboardKey::LeftMeta) || keyboard.IsKeyPressed(KeyboardKey::RightMeta));

		for (int i = 0; i < KeyboardState::KeyCount; i++)
		{
			KeyboardKey key = static_cast<KeyboardKey>(i);
            ImGuiKey imGuiKey = ToImGuiKey(key);

            if (imGuiKey == ImGuiKey_None)
                continue;

            io.AddKeyEvent(imGuiKey, keyboard.IsKeyPressed(key));
		}
        
        return true;
	}

    void ImGuiCocoPlatform::PlatformCreateWindow(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        WindowService* windowing = WindowService::Get();
        if (!windowing)
            throw std::exception("No active WindowService found");

        WindowCreateParams createParams("ImGui Window", SizeInt(static_cast<uint32>(viewport->Size.x), static_cast<uint32>(viewport->Size.y)));
        createParams.InitialPosition = Vector2Int(static_cast<int32>(viewport->Pos.x), static_cast<int32>(viewport->Pos.y));
        createParams.WithoutDecoration = true;

        Ref<Window> window = windowing->CreateWindow(createParams);
        CocoViewportData& viewportData = _sViewports.try_emplace(window->ID, window.Get()).first->second;

        viewport->PlatformHandle = static_cast<void*>(window.Get());
        viewport->PlatformUserData = &viewportData;
    }

    void ImGuiCocoPlatform::PlatformDestroyWindow(ImGuiViewport* viewport)
    {
        if (!viewport->PlatformHandle)
            return;

        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);

        auto it = _sViewports.find(window->ID);
        if (it != _sViewports.end())
        {
            _sViewports.erase(it);

            window->Close();

            viewport->PlatformHandle = nullptr;
            viewport->PlatformUserData = nullptr;
        }
    }

    void ImGuiCocoPlatform::PlatformShowWindow(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        window->Show();
    }

    void ImGuiCocoPlatform::PlatformSetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
    {
        using namespace Coco::Windowing;

        CocoViewportData* vd = static_cast<CocoViewportData*>(viewport->PlatformUserData);
        vd->IgnorePositionChanged = true;
        vd->Window->SetPosition(Vector2Int(static_cast<int>(pos.x), static_cast<int>(pos.y)), true, false);
    }

    ImVec2 ImGuiCocoPlatform::PlatformGetWindowPos(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        Vector2Int pos = window->GetPosition(true, false);
        return ImVec2(static_cast<float>(pos.X), static_cast<float>(pos.Y));
    }

    void ImGuiCocoPlatform::PlatformSetWindowSize(ImGuiViewport* viewport, ImVec2 size)
    {
        using namespace Coco::Windowing;

        CocoViewportData* vd = static_cast<CocoViewportData*>(viewport->PlatformUserData);
        vd->IgnoreSizeChanged = true;
        vd->Window->SetClientAreaSize(SizeInt(static_cast<uint32>(size.x), static_cast<uint32>(size.y)));
    }

    ImVec2 ImGuiCocoPlatform::PlatformGetWindowSize(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        SizeInt size = window->GetClientAreaSize();
        return ImVec2(static_cast<float>(size.Width), static_cast<float>(size.Height));
    }

    void ImGuiCocoPlatform::PlatformFocusWindow(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        window->Focus();
    }

    bool ImGuiCocoPlatform::PlatformWindowHasFocus(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        return window->HasFocus();
    }

    bool ImGuiCocoPlatform::PlatformWindowMinimized(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        return window->GetState() == WindowState::Minimized;
    }

    void ImGuiCocoPlatform::PlatformSetWindowTitle(ImGuiViewport* viewport, const char* title)
    {
        using namespace Coco::Windowing;

        Window* window = static_cast<Window*>(viewport->PlatformHandle);
        window->SetTitle(title);
    }

    void ImGuiCocoPlatform::PlatformRenderViewport(ImGuiViewport* viewport, void* renderArgs)
    {
        using namespace Coco::Rendering;
        using namespace Coco::Windowing;

        ImGuiIO& io = ::ImGui::GetIO();

        RenderService* rendering = RenderService::Get();
        if (!rendering)
            throw std::exception("No active RenderService found");

        ImGuiCocoPlatform* platform = static_cast<ImGuiCocoPlatform*>(io.BackendPlatformUserData);
        Window* window = static_cast<Window*>(viewport->PlatformHandle);

        platform->_currentlyRenderingViewport = viewport;

        std::array<SceneDataProvider*, 1> sceneProviders = { platform };
        rendering->Render(window->GetPresenter(), *platform->_renderPipeline, *platform, sceneProviders);

        platform->_currentlyRenderingViewport = nullptr;
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
        platformIO.Platform_RenderWindow = &ImGuiCocoPlatform::PlatformRenderViewport;
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

        BlendState blendState{};
        blendState.ColorSourceFactor = BlendFactorMode::SourceAlpha;
        blendState.ColorDestinationFactor = BlendFactorMode::OneMinusSourceAlpha;
        blendState.ColorBlendOperation = BlendOperation::Add;

        blendState.AlphaSourceBlendFactor = BlendFactorMode::One;
        blendState.AlphaDestinationBlendFactor = BlendFactorMode::OneMinusSourceAlpha;
        blendState.AlphaBlendOperation = BlendOperation::Add;

        ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();
        _shader = resources.Create<Shader>("ImGui", ImGuiRenderPass::sPassName);
        _shader->AddRenderPassShader(
            RenderPassShader(
                0,
                ImGuiRenderPass::sPassName,
                {
                    ShaderStage("main", ShaderStageType::Vertex, "shaders/built-in/ImGui.vert.spv"),
                    ShaderStage("main", ShaderStageType::Fragment, "shaders/built-in/ImGui.frag.spv")
                },
                pipelineState,
                {
                    blendState
                },
                {
                    ShaderVertexAttribute("Position", BufferDataType::Float3),
                    ShaderVertexAttribute("Color", BufferDataType::Float4),
                    ShaderVertexAttribute("UV", BufferDataType::Float2)
                },
                {
                    ShaderDataUniform("projection", UniformScope::Global, ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
                },
                {
                    ShaderTextureUniform("sTexture", UniformScope::Instance, ShaderStageFlags::Fragment)
                }
            )
        );

        // Setup ImGui mesh
        _mesh = resources.Create<Mesh>("ImGui", true);

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
            ImageSamplerDescription());

        _texture->SetPixels(0, pixels, static_cast<uint64>(width) * height * GetPixelFormatChannelCount(ImagePixelFormat::RGBA8));

        io.Fonts->SetTexID(static_cast<void*>(_texture.Get()));

        // Setup ImGui material
        _material = resources.Create<Material>("ImGui", _shader);
        _material->SetTexture("sTexture", _texture);
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
    }
}