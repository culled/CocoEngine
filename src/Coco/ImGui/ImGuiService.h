//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_IMGUISERVICE_H
#define COCOENGINE_IMGUISERVICE_H
#include "Coco/Core/EngineService.h"
#include "Coco/Core/ProcessLoop/TickListener.h"
#include "Coco/Core/Types/Event.h"
#include "Coco/Rendering/RenderListener.h"

struct ImGuiViewport;
struct ImVec2;

namespace Coco
{
	class ImGuiInputLayer;
	class WindowService;
	class Window;
	class Shader;

	/// @brief Data for an ImGui viewport window
    struct ImGuiViewportData
    {
	    /// @brief The window for this viewport
        Ref<Window> ViewportWindow;

	    /// @brief Handler for the window's OnPositionChanged event
        EventListener<const Vector2i&> PositionChangedHandler;

	    /// @brief If true, the next OnPositionChanged event will be ignored
        bool SuppressPositionChangedEvent;

    	/// @brief Handler for the window's OnSizeChanged event
        EventListener<const Sizei&> SizeChangedHandler;

	    /// @brief If true, the next OnSizeChanged event will be ignored
        bool SuppressSizeChangedEvent;

    	/// @brief Handler for the window's OnCloseRequested event
        EventListener<bool&> CloseRequestedHandler;

	    /// @brief If true, the next OnCloseRequested event will be ignored
        bool SuppressCloseRequestedEvent;

        ImGuiViewportData(Ref<Window> viewportWindow);

	    /// @brief Handler for the OnPositionChanged window event
        /// @param newPosition The new window position
        /// @return True if the event should not be propagated further
        bool OnPositionChanged(const Vector2i& newPosition);

	    /// @brief Handler for the OnSizeChanged window event
        /// @param newSize The new window size
        /// @return True if the event should not be propagated further
        bool OnSizeChanged(const Sizei& newSize);

	    /// @brief Handler for the OnCloseRequested window event
        /// @param cancel If set to true, the window will not be closed
        /// @return True if the event should not be propagated further
        bool OnCloseRequested(bool& cancel);

	    /// @brief Submits the ImGui render data to the renderer
    	/// @param graph The render graph
    	/// @param scene The scene being rendered
    	void Render(RenderGraph& graph, RenderScene& scene);
    };

    /// @brief An EngineService that enables the usage of ImGui
    class ImGuiService : public EngineService
    {
    public:
	    /// @brief The order when the ImGui data will be rendered to the screen
    	static constexpr int ImGuiRenderOrder = 100;

	    /// @brief The tick order when the new ImGui frame will start
    	static constexpr int NewFrameTickOrder = -6000;

	    /// @brief The tick order when the ImGui frame will end
    	static constexpr int EndFrameTickOrder = 6000;

        ImGuiService(Engine* engine, bool enableViewports);
        ~ImGuiService();

    private:
    	static uint64 _fontTextureResourceID;

    	WindowService* _windowService;
    	SharedPtr<Texture> _fontTexture;
    	SharedPtr<ImGuiInputLayer> _inputLayer;
        Map<uint64, ImGuiViewportData> _viewports;
    	TickListener _newFrameTickListener;
    	TickListener _endFrameTickListener;
    	bool _shouldUpdateDisplays;
    	RenderListener _viewportRenderListener;

        /// @brief Callback for when ImGui is allocating memory
		/// @param sz The size of the memory
		/// @param user_data The user data
		/// @return The allocated memory
		//static void* ImGuiAllocCallback(size_t sz, void* user_data);

		/// @brief Callback for when ImGui is freeing memory
		/// @param ptr The memory to free
		/// @param user_data The user data
		//static void ImGuiFreeCallback(void* ptr, void* user_data);

		/// @brief ImGui callback for creating a window
		/// @param viewport The viewport
		static void PlatformCreateWindow(ImGuiViewport* viewport);

		/// @brief ImGui callback for destroying a window
		/// @param viewport The viewport
		static void PlatformDestroyWindow(ImGuiViewport* viewport);

		/// @brief ImGui callback for showing a window
		/// @param viewport The viewport
		static void PlatformShowWindow(ImGuiViewport* viewport);

		/// @brief ImGui callback for setting a window's position
		/// @param viewport The viewport
		/// @param pos The new window position
		static void PlatformSetWindowPos(ImGuiViewport* viewport, ImVec2 pos);

		/// @brief ImGui callback for getting a window's position
		/// @param viewport The viewport
		/// @return The window's position
		static ImVec2 PlatformGetWindowPos(ImGuiViewport* viewport);

		/// @brief ImGui callback for setting a window's size
		/// @param viewport The viewport
		/// @param size The new window size
		static void PlatformSetWindowSize(ImGuiViewport* viewport, ImVec2 size);

		/// @brief ImGui callback for getting a window's size
		/// @param viewport The viewport
		/// @return The window's size
		static ImVec2 PlatformGetWindowSize(ImGuiViewport* viewport);

		/// @brief ImGui callback for focusing a window
		/// @param viewport The viewport
		static void PlatformFocusWindow(ImGuiViewport* viewport);

		/// @brief ImGui callback for getting a window's focused state
		/// @param viewport The viewport
		/// @return True if the window has focus
		static bool PlatformWindowHasFocus(ImGuiViewport* viewport);

		/// @brief ImGui callback for getting a window's minimized state
		/// @param viewport The viewport
		/// @return True if a window is minimized
		static bool PlatformWindowMinimized(ImGuiViewport* viewport);

		/// @brief ImGui callback for setting a window's title
		/// @param viewport The viewport
		/// @param title The window title
		static void PlatformSetWindowTitle(ImGuiViewport* viewport, const char* title);

	    /// @brief Sets up the ImGui input layer
    	void SetupInput();

	    /// @brief Creates viewport data for an ImGui viewport and its associated window
    	/// @param viewport The ImGui viewport
    	/// @param window The associated window for the viewport
    	void CreateDataForViewport(ImGuiViewport* viewport, Ref<Window> window);

	    /// @brief Updates the ImGui displays
    	void UpdateDisplays();

	    /// @brief Rebuilds the ImGui font texture
    	void RebuildFontTexture();

	    /// @brief Tick handler for the new frame
        /// @param tickInfo The tick info
    	void NewFrame(const TickInfo& tickInfo);

    	/// @brief Tick handler for the end frame
    	/// @param tickInfo The tick info
    	void EndFrame(const TickInfo& tickInfo);

	    /// @brief Handler for rendering
    	/// @param targetID The ID of the surface being rendered
    	/// @param graph The render graph
    	/// @param scene The scene being rendered
    	void OnRender(uint64 targetID, RenderGraph& graph, RenderScene& scene);
    };
} // Coco

#endif //COCOENGINE_IMGUISERVICE_H