#pragma once

#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/Rendering/Providers/RenderViewProvider.h>
#include <Coco/Rendering/Providers/SceneDataProvider.h>

namespace Coco::ImGuiCoco
{
	using namespace Coco::Rendering;

	/// @brief The platform that integrates ImGui with the Engine
	class ImGuiCocoPlatform :
		public RenderViewProvider,
		public SceneDataProvider
	{
	private:
		Ref<Shader> _shader;
		Ref<Material> _material;
		Ref<Mesh> _mesh;
		Ref<Texture> _texture;

	public:
		ImGuiCocoPlatform();
		~ImGuiCocoPlatform();

		UniqueRef<RenderView> CreateRenderView(
			const CompiledRenderPipeline& pipeline,
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
	};
}