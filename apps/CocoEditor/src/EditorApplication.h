#pragma once

#include <Coco/Core/Application.h>
#include <Coco/Core/Types/Singleton.h>
#include <Coco/Windowing/Window.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/Refs.h>

#include "SelectionContext.h"
#include "Panels/ViewportPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"

// TEMPORARY
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/ECS/Scene.h>
#include <Coco/ECS/Entity.h>
// TEMPORARY

namespace Coco
{
    class EditorApplication :
        public Application,
        public Singleton<EditorApplication>
    {
    private:
        SelectionContext _selection;
        Ref<Windowing::Window> _mainWindow;
        ManagedRef<TickListener> _updateTickListener;
        ManagedRef<TickListener> _renderTickListener;
        std::vector<uint8> _fontData;

        UniqueRef<ViewportPanel> _viewport;
        UniqueRef<SceneHierarchyPanel> _scenePanel;
        UniqueRef<InspectorPanel> _inspectorPanel;
        EventHandler<const ViewportPanel&> _viewportClosedHandler;
        
        // TEMPORARY
        SharedRef<RenderPipeline> _pipeline;
        SharedRef<ECS::Scene> _mainScene;
        ECS::Entity _entity;
        ECS::Entity _entity2;
        ECS::Entity _cameraEntity;
        // TEMPORARY

    public:
        EditorApplication();
        ~EditorApplication();

        SelectionContext& GetSelection() { return _selection; }

        Ref<Windowing::Window> GetMainWindow() const { return _mainWindow; }

    private:
        void SetupServices();
        void CreateMainWindow();
        void SetupDefaultLayout();
        void CreateMainScene();

        void HandleUpdateTick(const TickInfo& tickInfo);
        void HandleRenderTick(const TickInfo& tickInfo);

        void ShowFileMenu();
        void ShowViewMenu();

        UniqueRef<ViewportPanel> CreateViewportPanel();
        void CloseViewportPanel();
        bool OnViewportPanelClosed(const ViewportPanel& panel);

        void NewScene();
        void OpenScene();
        void SaveSceneAs();

        void ChangeScenes(SharedRef<Scene> newScene);
    };
}