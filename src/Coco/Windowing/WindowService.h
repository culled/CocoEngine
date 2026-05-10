//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_WINDOWSERVICE_H
#define COCOENGINE_WINDOWSERVICE_H
#include "DisplayInfo.h"
#include "Window.h"
#include "WindowingEnginePlatform.h"
#include "Coco/Core/EngineService.h"
#include "Coco/Core/Types/Array.h"

namespace Coco
{
    /// @brief An EngineService that manages GUI windows
    class WindowService : public EngineService
    {
    public:
        WindowService(Engine* engine);
        ~WindowService();

        /// @brief Updates information for connected displays
        void UpdateDisplays();

        /// @brief Creates a new window. Call Window::Show() to actually make the window visible
        /// @param createParams The window create parameters
        /// @return The window
        Ref<Window> CreateWindow(const WindowCreateParams& createParams);

        /// @brief Gets the main window. The main window is always the first window created
        /// @return The main window, if one has been created
        Ref<Window> GetMainWindow() { return _mainWindow; }

        /// @brief Gets the number of visible windows
        /// @param includeInactive If true, also include the number of windows not currently visible
        /// @return The number of windows
        uint64 GetWindowCount(bool includeInactive) const;

        /// @brief Gets references to all visible windows
        /// @param includeInactive If true, also include windows not currently visible
        /// @param outWindows Will be filled with references to the windows. Must be as large as the value returned from GetWindowCount()
        void GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) const;

        /// @brief Gets the connected displays
        /// @return The connected displays
        Span<const DisplayInfo> GetDisplays() const { return _displays; }

    private:
        WindowingEnginePlatform* _windowingPlatform;
        Array<DisplayInfo> _displays;
        Ref<Window> _mainWindow;
    };
} // Coco

#endif //COCOENGINE_WINDOWSERVICE_H