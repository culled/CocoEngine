#include "ImGuiCocoPlatform.h"

#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/RenderService.h>

#include <Coco/Input/InputService.h>

#include <Coco/Windowing/WindowService.h>
#include <Coco/Windowing/Window.h>

#include <Coco/Core/Engine.h>

#include "ImGuiRenderViewProvider.h"
#include "ImGuiSceneDataProvider.h"

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

    ImGuiCocoPlatform::ImGuiCocoPlatform(bool enableViewports, bool clearAttachments) :
        _renderPass(CreateSharedRef<ImGuiRenderPass>()),
        _shouldUpdateDisplays(true)
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

        CreateObjects(clearAttachments);
        RebuildFontTexture();
    }

    ImGuiCocoPlatform::~ImGuiCocoPlatform()
    {
        ImGuiIO& io = ::ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) == ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::DestroyPlatformWindows();
            ShutdownPlatformInterface();
        }

        _renderPipeline.reset();
        _renderPass.reset();
    }

    bool ImGuiCocoPlatform::NewFrame(const TickInfo& tickInfo)
	{
		using namespace Coco::Windowing;

		WindowService* windowing = WindowService::Get();
        CocoAssert(windowing, "WindowService singleton was null")

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
        CocoAssert(rendering, "RenderService singleton was null")

        UniqueRef<ImGuiRenderViewProvider> viewProvider = CreateUniqueRef<ImGuiRenderViewProvider>(viewport);
        UniqueRef<ImGuiSceneDataProvider> sceneProvider = CreateUniqueRef<ImGuiSceneDataProvider>(viewport);

        std::array<SceneDataProvider*, 1> sceneProviders = { sceneProvider.get() };

        rendering->Render(viewport->ID, window->GetPresenter(), *_renderPipeline, *viewProvider, sceneProviders);
    }

    void ImGuiCocoPlatform::RebuildFontTexture()
    {
        // Setup the font texture
        int width, height;
        unsigned char* pixels = nullptr;
        ImGuiIO& io = ::ImGui::GetIO();

        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        _texture = Engine::Get()->GetResourceLibrary().Create<Texture>(
            ResourceID("ImGuiCocoPlatform::FontTexture"),
            ImageDescription::Create2D(
                static_cast<uint32>(width), static_cast<uint32>(height),
                ImagePixelFormat::RGBA8, ImageColorSpace::sRGB,
                ImageUsageFlags::Sampled,
                false
            ),
            ImageSamplerDescription::LinearClamp);

        _texture->SetPixels(0, pixels, static_cast<uint64>(width) * height * GetPixelFormatChannelCount(ImagePixelFormat::RGBA8));

        uint64 textureID = _texture->GetID();
        io.Fonts->SetTexID((void*)textureID);
    }

    void ImGuiCocoPlatform::PlatformCreateWindow(ImGuiViewport* viewport)
    {
        using namespace Coco::Windowing;

        WindowService* windowing = WindowService::Get();
        CocoAssert(windowing, "WindowService singleton was null")

        WindowCreateParams createParams("ImGui Window", SizeInt(static_cast<uint32>(viewport->Size.x), static_cast<uint32>(viewport->Size.y)));
        createParams.InitialPosition = Vector2Int(static_cast<int32>(viewport->Pos.x), static_cast<int32>(viewport->Pos.y));

        if((viewport->Flags & ImGuiViewportFlags_NoDecoration) == ImGuiViewportFlags_NoDecoration)
            createParams.StyleFlags |= WindowStyleFlags::NoDecoration;
        
        if((viewport->Flags & ImGuiViewportFlags_TopMost) == ImGuiViewportFlags_TopMost)
            createParams.StyleFlags |= WindowStyleFlags::TopMost;
        
        if ((viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon) == ImGuiViewportFlags_NoTaskBarIcon)
            createParams.StyleFlags |= WindowStyleFlags::NoTaskbarIcon;

        if ((viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing) == ImGuiViewportFlags_NoFocusOnAppearing)
            createParams.FocusOnShow = false;

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

                ImGuiSceneDataProvider::DestroyViewportMesh(viewport);

                viewport->PlatformHandle = nullptr;
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

    void ImGuiCocoPlatform::ShutdownPlatformInterface()
    {
        ImGuiPlatformIO& platformIO = ::ImGui::GetPlatformIO();
        platformIO.Platform_CreateWindow = nullptr;
        platformIO.Platform_DestroyWindow = nullptr;
        platformIO.Platform_ShowWindow = nullptr;
        platformIO.Platform_SetWindowPos = nullptr;
        platformIO.Platform_GetWindowPos = nullptr;
        platformIO.Platform_SetWindowSize = nullptr;
        platformIO.Platform_GetWindowSize = nullptr;
        platformIO.Platform_SetWindowFocus = nullptr;
        platformIO.Platform_GetWindowFocus = nullptr;
        platformIO.Platform_GetWindowMinimized = nullptr;
        platformIO.Platform_SetWindowTitle = nullptr;
    }

    void ImGuiCocoPlatform::CreateObjects(bool clearAttachments)
    {
        ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

        // Create the pipeline
        _renderPipeline = resources.Create<Rendering::RenderPipeline>(
            ResourceID("ImGuiCocoPlatform::RenderPipeline"), 
            clearAttachments ? AttachmentOptionsFlags::Clear : AttachmentOptionsFlags::None);

        std::array<uint32, 1> bindings = { 0 };
        _renderPipeline->AddRenderPass(_renderPass, bindings);
    }

    void ImGuiCocoPlatform::UpdateDisplays()
    {
        using namespace Coco::Windowing;

        WindowService* windowing = WindowService::Get();
        CocoAssert(windowing, "WindowService singleton was null")

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
}