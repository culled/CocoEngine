#pragma once

#include <Coco/Rendering/RenderingResource.h>

#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>
#include "../RenderView.h"
#include "RenderContextTypes.h"

namespace Coco::Rendering
{
	class IRenderPass;
	class RenderPipeline;
	class Shader;
	class Material;
	class Mesh;

	/// @brief A context that can be used for rendering
	class COCOAPI RenderContext : public RenderingResource
	{
		friend RenderPipeline;

	protected:
		/// @brief The render view used for rendering
		Ref<RenderView> _currentRenderView;

		/// @brief The render pipeline being used for rendering
		Ref<RenderPipeline> _currentRenderPipeline;

		/// @brief The currently rendering render pass
		Ref<IRenderPass> _currentRenderPass = Ref<IRenderPass>::Empty;

		/// @brief The index of the current render pass in the pipeline
		int _currentRenderPassIndex = -1;

		/// @brief The currently bound shader render data
		ShaderRenderData _currentShader;

		/// @brief The current global uniform data
		ShaderUniformData _currentGlobalUniformData;

		/// @brief The current instance uniform data
		ShaderUniformData _currentInstanceUniformData;

		/// @brief The currently bound material render data
		MaterialRenderData _currentMaterial;

		/// @brief The number of draw calls this render
		uint64_t _currentDrawCallCount = 0;

		/// @brief The number of triangles drawn this render
		uint64_t _currentTrianglesDrawn = 0;

	protected:
		RenderContext(const ResourceID& id, const string& name);

	public:
		virtual ~RenderContext() = default;

		RenderContext(const RenderContext&) = delete;
		RenderContext(RenderContext&&) = delete;

		RenderContext& operator=(const RenderContext&) = delete;
		RenderContext& operator=(RenderContext&&) = delete;

		/// @brief Gets the RenderView that this context is rendering with
		/// @return The current RenderView (only valid between Start() and End() calls)
		Ref<Rendering::RenderView> GetRenderView() const noexcept { return _currentRenderView; }
		
		/// @brief Begins rendering for a scene
		/// @param renderView The view used for rendering
		/// @param pipeline The render pipeline to use
		/// @return True if rendering began successfully
		bool Begin(Ref<Rendering::RenderView> renderView, Ref<RenderPipeline> pipeline);

		/// @brief Begins rendering for a render pass
		/// @param renderPass The render pass
		/// @return True if renderpass rendering began successfully
		bool BeginPass(Ref<IRenderPass> renderPass);

		/// @brief Ends rendering for a scene
		void End();

		/// @brief Resets this context to begin rendering a new scene
		void Reset();

		/// @brief Sets the rectangle of the viewport to use
		/// @param rect The viewport rectangle
		virtual void SetViewport(const RectInt& rect) = 0;

		/// @brief Restores the viewport to the size and offset specified by the RenderView
		void RestoreViewport();

		/// @brief Sets the given shader as the one that will be used to draw subsequent geometry
		/// @param shaderID The ID of the shader to use
		void UseShader(const ResourceID& shaderID);

		/// @brief Resets the current shader uniform data
		void ResetShaderUniformData();

		/// @brief Sets a Vector4 uniform
		/// @param scope The scope of the uniform
		/// @param name The uniform name
		/// @param value The value
		void SetShaderVector4(ShaderDescriptorScope scope, const string& name, const Vector4& value);

		/// @brief Sets a Color uniform
		/// @param scope The scope of the uniform
		/// @param name The uniform name
		/// @param value The value
		void SetShaderColor(ShaderDescriptorScope scope, const string& name, const Color& value);

		/// @brief Sets a Matrix4x4 uniform
		/// @param scope The scope of the uniform
		/// @param name The uniform name
		/// @param value The value
		void SetShaderMatrix4x4(ShaderDescriptorScope scope, const string& name, const Matrix4x4& value);

		/// @brief Sets a Texture sampler
		/// @param scope The scope of the sampler
		/// @param name The sampler name
		/// @param textureID The ID of the texture resource
		void SetShaderTexture(ShaderDescriptorScope scope, const string& name, const ResourceID& textureID);

		/// @brief Sets the given material as the one that will be used to draw subsequent geometry
		/// @param materialID The ID of the material to use
		void UseMaterial(const ResourceID& materialID);

		/// @brief Draws an object
		/// @param objectData The object to draw
		virtual void Draw(const ObjectRenderData& objectData) = 0;

		/// @brief Gets if this render context can be immediately used for rendering
		/// @return True if this render context can immediately be used for rendering
		virtual bool IsAvaliableForRendering() noexcept = 0;

		/// @brief Waits for this render context's rendering to complete
		virtual void WaitForRenderingCompleted() = 0;

		/// @brief Gets the drawing stats for the last frame
		/// @param drawCallCount Will be set to the number of draw calls
		/// @param trianglesDrawn Will be set to the number of triangles
		void GetLastFrameStats(uint64_t& drawCallCount, uint64_t trianglesDrawn) const noexcept;

	protected:
		/// @brief Called when this render context is starting to render a scene
		/// @return True if rendering started successfully
		virtual bool BeginImpl() = 0;

		virtual bool BeginPassImpl() = 0;

		/// @brief Called when this render context ends rendering the current scene
		virtual void EndImpl() = 0;

		/// @brief Called when this render context should reset
		virtual void ResetImpl() = 0;

		/// @brief Called after a new shader has been bound
		virtual void NewShaderBound() {}

		/// @brief Called after a new material has been bound
		virtual void NewMaterialBound() {}

		/// @brief Called after a uniform's data has been changed
		/// @param name The name of the uniform
		virtual void UniformUpdated(const string& name) {}

		/// @brief Called after a texture sampler's data has been changed
		/// @param name The name of the texture sampler
		virtual void TextureSamplerUpdated(const string& name) {}
	};
}