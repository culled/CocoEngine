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

    struct ImGuiViewportData
    {
        Ref<Window> ViewportWindow;

        EventListener<const Vector2i&> PositionChangedHandler;
        bool SuppressPositionChangedEvent;

        EventListener<const Sizei&> SizeChangedHandler;
        bool SuppressSizeChangedEvent;

        EventListener<bool&> CloseRequestedHandler;
        bool SuppressCloseRequestedEvent;

        ImGuiViewportData(Ref<Window> viewportWindow);

        bool OnPositionChanged(const Vector2i& newPosition);
        bool OnSizeChanged(const Sizei& newSize);
        bool OnClosing(bool& cancel);
    	void Render(RenderGraph& graph, RenderScene& scene);
    };

    class ImGuiService : public EngineService
    {
    public:
    	static constexpr int ImGuiRenderOrder = 100;
    	static constexpr int NewFrameOrder = -6000;
    	static constexpr int EndFrameOrder = 6000;

    public:
        ImGuiService(Engine* engine, bool enableViewports);
        ~ImGuiService();

    	SharedPtr<Shader> GetImGuiShader() { return _shader; }

    private:
    	static uint64 _fontTextureResourceID;

    	WindowService* _windowService;
    	SharedPtr<Texture> _fontTexture;
    	SharedPtr<ImGuiInputLayer> _inputLayer;
        Map<uint64, ImGuiViewportData> _viewports;
    	TickListener _newFrameTickListener;
    	TickListener _endFrameTickListener;
    	bool _shouldUpdateDisplays;
    	SharedPtr<Shader> _shader;
    	RenderListener _viewportRenderListener;

    private:
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

    	void CreateResources();
    	void SetupInput();
    	void CreateDataForViewport(ImGuiViewport* viewport, Ref<Window> window);
    	void UpdateDisplays();
    	void RebuildFontTexture();

        void NewFrame(const TickInfo& tickInfo);
    	void EndFrame(const TickInfo& tickInfo);
    	void OnRender(uint64 targetID, RenderGraph& graph, RenderScene& scene);
    };
} // Coco

#endif //COCOENGINE_IMGUISERVICE_H