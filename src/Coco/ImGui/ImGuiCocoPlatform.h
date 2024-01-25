#pragma once

#include <Coco/Core/Types/Singleton.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/Core/Events/Event.h>

#include <Coco/Rendering/Providers/SceneDataProvider.h>

#include <Coco/Windowing/DisplayInfo.h>

#include <Coco/Input/InputTypes.h>

#include "ImGuiRenderPass.h"

struct ImGuiViewport;
struct ImVec2;

namespace Coco::Rendering
{
	class Material;
	class Shader;
	class Texture;
	class RenderPipeline;
}

namespace Coco::Windowing
{
	class Window;
}

namespace Coco::ImGuiCoco
{
	using namespace Coco::Rendering;

	/// @brief Data for an ImGui viewport
	struct CocoViewportData
	{
		/// @brief The window that holds the viewport
		Windowing::Window* Window;

		/// @brief A handler for the window's position changed event
		EventHandler<const Vector2Int&> OnPositionChanged;

		/// @brief If true, the callback for the position changed event will be suppressed the next time the event fires
		bool IgnorePositionChanged;

		/// @brief A handler for the window's size changed event
		EventHandler<const SizeInt&> OnSizeChanged;

		/// @brief If true, the callback for the size changed event will be suppressed the next time the event fires
		bool IgnoreSizeChanged;

		/// @brief A handler for the window's closing event
		EventHandler<bool&> OnClosing;

		CocoViewportData(Windowing::Window* window);
		~CocoViewportData();
	};

	/// @brief The platform that integrates ImGui with the Engine
	class ImGuiCocoPlatform :
		public Singleton<ImGuiCocoPlatform>
	{
		friend CocoViewportData;

	public:
		ImGuiCocoPlatform(bool enableViewports, bool clearAttachments);
		~ImGuiCocoPlatform();

		/// @brief Sets up the platform for a new frame
		/// @param tickInfo The info for the current tick
		/// @return True if the new frame was initialized successfully
		bool NewFrame(const TickInfo& tickInfo);

		/// @brief Gets the ImGui font texture
		/// @return The ImGui font texture
		SharedRef<Texture> GetTexture() const { return _texture; }

		/// @brief Renders a viewport
		/// @param viewport The viewport
		void RenderViewport(ImGuiViewport* viewport);

		/// @brief Rebuilds the internal font texture. Call this if a font in ImGui has changed
		void RebuildFontTexture();

	private:
		static std::unordered_map<uint64, CocoViewportData> _sViewports;

		std::vector<Windowing::DisplayInfo> _displays;
		bool _shouldUpdateDisplays;
		SharedRef<Texture> _texture;
		SharedRef<Shader> _shader;
		SharedRef<ImGuiRenderPass> _renderPass;
		SharedRef<Rendering::RenderPipeline> _renderPipeline;

	private:
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

		/// @brief Called when a window is moved
		/// @param window The window
		static void PlatformWindowPositionChangedCallback(Windowing::Window* window);

		/// @brief Called when a window is resized
		/// @param window The window
		static void PlatformWindowSizeChangedCallback(Windowing::Window* window);

		/// @brief Called when a window tries to close
		/// @param window The window
		static void PlatformWindowCloseCallback(Windowing::Window* window);

		/// @brief Initializes the platform interface for ImGui
		void InitPlatformInterface();

		/// @brief Shuts down the platform interface for ImGui
		void ShutdownPlatformInterface();

		/// @brief Creates resources for ImGui rendering
		/// @param clearAttachments If true, attachments will be cleared
		void CreateObjects(bool clearAttachments);

		/// @brief Updates the ImGui displays
		void UpdateDisplays();
	};
}