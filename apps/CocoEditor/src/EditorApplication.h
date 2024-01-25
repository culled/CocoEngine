#pragma once

#include <Coco/Core/Application.h>
#include <Coco/Core/Types/Singleton.h>
#include <Coco/Windowing/Window.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include <Coco/Core/Events/Event.h>
#include <Coco/Core/Types/Refs.h>

#include "SelectionContext.h"
#include "EditorInputLayer.h"
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
    public:
        EditorApplication();
        ~EditorApplication();

        SelectionContext& GetSelection() { return _selection; }
        Ref<Windowing::Window> GetMainWindow() const { return _mainWindow; }

        void NewScene();
        void OpenScene();
        void OpenScene(const FilePath& scenePath);
        void SaveSceneAs();

        void StartPlayInEditor();
        bool IsPlayingInEditor() const { return _mainScene->GetSimulateMode() == ECS::SceneSimulateMode::Running; }
        void StopPlayInEditor();

    private:
        SelectionContext _selection;
        ManagedRef<EditorInputLayer> _inputLayer;
        Ref<Windowing::Window> _mainWindow;
        ManagedRef<TickListener> _updateTickListener;
        ManagedRef<TickListener> _renderTickListener;
        std::vector<uint8> _fontData;

        UniqueRef<SceneHierarchyPanel> _sceneHierarchyPanel;
        UniqueRef<InspectorPanel> _inspectorPanel;

        // TEMPORARY
        SharedRef<Rendering::RenderPipeline> _renderPipeline;
        SharedRef<ECS::Scene> _mainScene;
        // TEMPORARY

    private:
        void SetupServices();
        void CreateMainWindow();
        void CreateResources();
        void CreateMainScene();
        void CreatePanels();
        void RegisterComponentUI();

        void HandleUpdateTick(const TickInfo& tickInfo);
        void HandleRenderTick(const TickInfo& tickInfo);

        void DrawUI();

        void ShowFileMenu();
        void ShowViewMenu();

        void ChangeScenes(SharedRef<ECS::Scene> newScene);
    };
}