#pragma once

#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include <Coco/Windowing/DisplayInfo.h>
#include <Coco/Windowing/Window.h>
#include <Coco/Core/Events/Event.h>
#include "ImGuiRenderPass.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

struct ImGuiViewport;
struct ImVec2;

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
		public RenderViewProvider,
		public SceneDataProvider
	{
		friend CocoViewportData;

	private:
		static std::unordered_map<uint64, CocoViewportData> _sViewports;

		std::vector<Windowing::DisplayInfo> _displays;
		Ref<Shader> _shader;
		Ref<Material> _material;
		Ref<Mesh> _mesh;
		Ref<Texture> _texture;
		SharedRef<ImGuiRenderPass> _renderPass;
		UniqueRef<Rendering::RenderPipeline> _renderPipeline;
		ImGuiViewport* _currentlyRenderingViewport;

	public:
		ImGuiCocoPlatform();
		~ImGuiCocoPlatform();

		UniqueRef<RenderView> CreateRenderView(
			const CompiledRenderPipeline& pipeline,
			uint64 rendererID,
			const SizeInt& backbufferSize,
			std::span<Ref<Image>> backbuffers) final;

		void GatherSceneData(RenderView& renderView) final;

		/// @brief Sets up the platform for a new frame
		/// @param tickInfo The info for the current tick
		/// @return True if the new frame was initialized successfully
		bool NewFrame(const TickInfo& tickInfo);

		/// @brief Gets the ImGui material
		/// @return The ImGui material
		Ref<Material> GetMaterial() const { return _material; }

		/// @brief Gets the ImGui mesh
		/// @return The ImGui mesh
		Ref<Mesh> GetMesh() const { return _mesh; }

		/// @brief Gets the ImGui font texture
		/// @return The ImGui font texture
		Ref<Texture> GetTexture() const { return _texture; }

		/// @brief Gets the ImGui shader
		/// @return The ImGui shader
		Ref<Shader> GetShader() const { return _shader; }

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

		/// @brief ImGui callback for rendering for a winodw
		/// @param viewport The viewport
		/// @param renderArgs Arguments for a render
		static void PlatformRenderViewport(ImGuiViewport* viewport, void* renderArgs);

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

		/// @brief Creates resources for ImGui rendering
		void CreateObjects();

		/// @brief Updates the ImGui displays
		void UpdateDisplays();
	};
}