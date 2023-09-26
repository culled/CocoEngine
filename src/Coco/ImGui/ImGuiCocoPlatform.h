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

	struct CocoViewportData
	{
		Windowing::Window* Window;

		EventHandler<const Vector2Int&> OnPositionChanged;
		bool IgnorePositionChanged;

		EventHandler<const SizeInt&> OnSizeChanged;
		bool IgnoreSizeChanged;

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
		static void PlatformCreateWindow(ImGuiViewport* viewport);
		static void PlatformDestroyWindow(ImGuiViewport* viewport);
		static void PlatformShowWindow(ImGuiViewport* viewport);
		static void PlatformSetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
		static ImVec2 PlatformGetWindowPos(ImGuiViewport* viewport);
		static void PlatformSetWindowSize(ImGuiViewport* viewport, ImVec2 size);
		static ImVec2 PlatformGetWindowSize(ImGuiViewport* viewport);
		static void PlatformFocusWindow(ImGuiViewport* viewport);
		static bool PlatformWindowHasFocus(ImGuiViewport* viewport);
		static bool PlatformWindowMinimized(ImGuiViewport* viewport);
		static void PlatformSetWindowTitle(ImGuiViewport* viewport, const char* title);
		static void PlatformRenderViewport(ImGuiViewport* viewport, void* renderArgs);

		static void PlatformWindowPositionChangedCallback(Windowing::Window* window);
		static void PlatformWindowSizeChangedCallback(Windowing::Window* window);
		static void PlatformWindowCloseCallback(Windowing::Window* window);

		void InitPlatformInterface();
		void CreateObjects();

		void UpdateDisplays();
	};
}