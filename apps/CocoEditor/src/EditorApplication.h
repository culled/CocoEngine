#pragma once

#include <Coco/Core/Application.h>
#include <Coco/Core/Types/Singleton.h>
#include <Coco/Windowing/Window.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/Refs.h>

#include "Panels/ViewportPanel.h"

// TEMPORARY
#include "RenderTest.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
// TEMPORARY

namespace Coco
{
    class EditorApplication :
        public Application,
        public Singleton<EditorApplication>
    {
    private:
        Ref<Windowing::Window> _mainWindow;
        ManagedRef<TickListener> _updateTickListener;
        ManagedRef<TickListener> _renderTickListener;

        UniqueRef<ViewportPanel> _viewport;
        EventHandler<const ViewportPanel&> _viewportClosedHandler;
        
        // TEMPORARY
        UniqueRef<RenderTest> _renderTest;
        UniqueRef<RenderPipeline> _pipeline;
        // TEMPORARY

    public:
        EditorApplication();
        ~EditorApplication();

        Ref<Windowing::Window> GetMainWindow() const { return _mainWindow; }

    private:
        void SetupServices();
        void CreateMainWindow();
        void SetupDefaultLayout();

        void HandleUpdateTick(const TickInfo& tickInfo);
        void HandleRenderTick(const TickInfo& tickInfo);

        void ShowFileMenu();
        void ShowViewMenu();

        UniqueRef<ViewportPanel> CreateViewportPanel();
        void CloseViewportPanel();
        bool OnViewportPanelClosed(const ViewportPanel& panel);
    };
}