#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Stopwatch.h>
#include "RenderContextTypes.h"
#include "GraphicsFence.h"
#include "GraphicsSemaphore.h"
#include "ShaderUniformData.h"
#include "ShaderUniformLayout.h"

namespace Coco::Rendering
{
	struct CompiledRenderPipeline;
	struct RenderPassBinding;
	class RenderView;
	struct ShaderData;
	struct MaterialData;
	struct MeshData;

	/// @brief Holds data that a RenderContext uses during actual rendering
	struct ContextRenderOperation
	{
		/// @brief The pipeline being rendered with
		CompiledRenderPipeline& Pipeline;

		/// @brief The index of the current RenderPass in the pipeline
		uint32 CurrentPassIndex;

		/// @brief The name of the current RenderPass in the pipeline
		string CurrentPassName;

		ContextRenderOperation(CompiledRenderPipeline& pipeline);

		/// @brief Gets the current pass
		/// @return The current pass
		const RenderPassBinding& GetCurrentPass() const;

		/// @brief Increments the current pass index
		void NextPass();
	};

	/// @brief A context that performs rendering operations
	class RenderContext
	{
	public:

	protected:
		RenderContextState _currentState;
		RenderView* _renderView;
		std::optional<ContextRenderOperation> _renderOperation;
		RenderContextRenderStats _stats;
		Stopwatch _executionStopwatch;
		Stopwatch _currentPassStopwatch;

		ShaderUniformData _globalUniforms;
		ShaderUniformData _globalShaderUniforms;
		ShaderUniformData _instanceUniforms;
		ShaderUniformData _drawUniforms;

	protected:
		RenderContext();

	public:
		virtual ~RenderContext() = default;

		/// @brief Blocks until this context's rendering operations have finished
		virtual void WaitForRenderingToComplete() = 0;

		/// @brief Checks and updates this RenderContext's state if it has completed the last render
		/// @return True if this context has completed its last render
		virtual bool CheckForRenderingComplete() = 0;

		/// @brief Gets the semaphore used by this context that can be used to wait before it starts the render
		/// @return The semaphore
		virtual Ref<GraphicsSemaphore> GetRenderStartSemaphore() = 0;

		/// @brief Gets the semaphore used to wait until rendering with this context has completed
		/// @return The semaphore
		virtual Ref<GraphicsSemaphore> GetRenderCompletedSemaphore() = 0;

		/// @brief Gets the fence that can be used to wait until rendering with this context has completed
		/// @return The fence
		virtual Ref<GraphicsFence> GetRenderCompletedFence() = 0;

		/// @brief Adds a semaphore that this context will wait on before executing
		/// @param semaphore The semaphore
		virtual void AddWaitOnSemaphore(Ref<GraphicsSemaphore> semaphore) = 0;

		/// @brief Adds a semaphore that this context will signal once rendering is completed
		/// @param semaphore The semaphore
		virtual void AddRenderCompletedSignalSemaphore(Ref<GraphicsSemaphore> semaphore) = 0;

		/// @brief Sets the viewport rectangle. Only valid during rendering
		/// @param viewportRect The viewport rectangle
		virtual void SetViewportRect(const RectInt& viewportRect) = 0;

		/// @brief Sets the part of the viewport that will be rendered to. Only valid during rendering
		/// @param scissorRect The scissor rectangle
		virtual void SetScissorRect(const RectInt& scissorRect) = 0;

		/// @brief Sets the shader that should be used for subsequent draw calls
		/// @param shader The shader
		/// @param variantName The name of the shader variant
		virtual void SetShader(const ShaderData& shader, const string& variantName) = 0;

		/// @brief Sets the material that should be used for subsequent draw calls
		/// @param material The material
		/// @param shaderVariantName The name of the shader variant that should be bound
		virtual void SetMaterial(const MaterialData& material, const string& shaderVariantName) = 0;

		/// @brief Draws a number of indices of a mesh
		/// @param mesh The mesh
		/// @param firstIndexOffset The offset of the first index in the mesh's index buffer
		/// @param indexCount The number of indices to draw
		virtual void DrawIndexed(const MeshData& mesh, uint64 firstIndexOffset, uint64 indexCount) = 0;

		/// @brief Sets the current RenderView
		/// @param renderView The render view
		void SetCurrentRenderView(RenderView& renderView);

		/// @brief Gets the current render stats. Gets reset via Reset()
		/// @return The render stats
		const RenderContextRenderStats& GetRenderStats() const { return _stats; }

		/// @brief Gets the platform seconds when this context last started a render
		/// @return The platform seconds
		double GetLastRenderStartTime() const { return _executionStopwatch.GetStartTimeSeconds(); }

		/// @brief Gets this RenderContext's state
		/// @return The state
		RenderContextState GetState() const { return _currentState; }

		/// @brief Sets a float uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetFloat(UniformScope scope, ShaderUniformData::UniformKey key, float value);

		/// @brief Sets a float2 uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetFloat2(UniformScope scope, ShaderUniformData::UniformKey key, const Vector2& value);

		/// @brief Sets a float3 uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetFloat3(UniformScope scope, ShaderUniformData::UniformKey key, const Vector3& value);

		/// @brief Sets a float4 uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetFloat4(UniformScope scope, ShaderUniformData::UniformKey key, const Vector4& value);

		/// @brief Sets a float uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		/// @param asLinear If true, the color will be written in a linear color gamut instead of gamma
		void SetFloat4(UniformScope scope, ShaderUniformData::UniformKey key, const Color& value, bool asLinear = true);

		/// @brief Sets a Mat4x4 uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetMatrix4x4(UniformScope scope, ShaderUniformData::UniformKey key, const Matrix4x4& value);

		/// @brief Sets an int uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetInt(UniformScope scope, ShaderUniformData::UniformKey key, int32 value);

		/// @brief Sets an int2 uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetInt2(UniformScope scope, ShaderUniformData::UniformKey key, const Vector2Int& value);

		/// @brief Sets an int3 uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetInt3(UniformScope scope, ShaderUniformData::UniformKey key, const Vector3Int& value);

		/// @brief Sets an int4 uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetInt4(UniformScope scope, ShaderUniformData::UniformKey key, const Vector4Int& value);

		/// @brief Sets a bool uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param value The value
		void SetBool(UniformScope scope, ShaderUniformData::UniformKey key, bool value);

		/// @brief Sets a TextureSampler uniform.
		/// NOTE: bind a shader before calling this to make sure instance & draw data get set properly
		/// @param scope The scope of the uniform
		/// @param key The uniform key
		/// @param image The image
		/// @param sampler The image sampler
		void SetTextureSampler(UniformScope scope, ShaderUniformData::UniformKey key, const Ref<Image>& image, const Ref<ImageSampler>& sampler);

		/// @brief Sets global buffer data
		/// @param key The uniform key
		/// @param offset The offset of the first byte of data in the buffer
		/// @param data The data
		/// @param dataSize The size of the data
		virtual void SetGlobalBufferData(ShaderUniformData::UniformKey key, uint64 offset, const void* data, uint64 dataSize) = 0;

		/// @brief Sets shader global buffer data.
		/// NOTE: bind a shader before calling this
		/// @param key The uniform key
		/// @param offset The offset of the first byte of data in the buffer
		/// @param data The data
		/// @param dataSize The size of the data
		virtual void SetShaderGlobalBufferData(ShaderUniformData::UniformKey key, uint64 offset, const void* data, uint64 dataSize) = 0;

		/// @brief Begins rendering
		/// @param pipeline The pipeline to render with
		/// @return True if this context is ready for rendering
		bool Begin(CompiledRenderPipeline& pipeline);

		/// @brief Begins the next render pass
		/// @return True if this context is ready for rendering
		bool BeginNextPass();

		/// @brief Ends rendering with this context
		void End();

		/// @brief Resets this context for beginning a new render after the previous has finished
		void Reset();

	protected:
		/// @brief Called when this context is starting the first render pass
		/// @return True if this context is ready for rendering
		virtual bool BeginImpl() = 0;

		/// @brief Called when this context should advance to the next render pass
		/// @return True if this context is ready for rendering
		virtual bool BeginNextPassImpl() = 0;

		/// @brief Called when this context has finished rendering
		virtual void EndImpl() = 0;

		/// @brief Called when this context is reset
		virtual void ResetImpl() = 0;

		/// @brief Called when a uniform changes
		/// @param scope The scope of the uniform
		/// @param key The key of the uniform
		virtual void UniformChanged(UniformScope scope, ShaderUniformData::UniformKey key) = 0;
	};
}