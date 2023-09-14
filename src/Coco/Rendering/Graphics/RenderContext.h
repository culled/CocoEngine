#pragma once

#include <Coco/Core/Types/Refs.h>
#include "GraphicsFence.h"
#include "GraphicsSemaphore.h"
#include "RenderView.h"

namespace Coco::Rendering
{
	struct CompiledRenderPipeline;
	struct RenderPassBinding;

	/// @brief Holds data that a RenderContext uses during actual rendering
	struct ContextRenderOperation
	{
		/// @brief The view being rendered to
		RenderView& RenderView;

		/// @brief The pipeline being rendered with
		CompiledRenderPipeline& Pipeline;

		/// @brief The index of the RenderPass in the pipeline
		uint32 CurrentPassIndex;

		ContextRenderOperation(Rendering::RenderView& renderView, CompiledRenderPipeline& pipeline);

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
		/// @brief States for a RenderContext
		enum class State
		{
			NeedsReset,
			ReadyForRender,
			InRender,
			EndedRender
		};

	protected:
		State _currentState;
		std::optional<ContextRenderOperation> _renderOperation;

	protected:
		RenderContext();

	public:
		virtual ~RenderContext() = default;

		/// @brief Blocks until this context is idle and ready to be rendered to
		virtual void WaitForRenderingToComplete() = 0;

		/// @brief Gets the semaphore used to wait until an image becomes available for rendering
		/// @return The semaphore
		virtual GraphicsSemaphore* GetImageAvailableSemaphore() = 0;

		/// @brief Gets the semaphore used to wait until rendering with this context has completed
		/// @return The semaphore
		virtual GraphicsSemaphore* GetRenderCompletedSemaphore() = 0;

		/// @brief Sets the viewport rectangle. Only valid during rendering
		/// @param viewportRect The viewport rectangle
		virtual void SetViewportRect(const RectInt& viewportRect) = 0;

		/// @brief Sets the part of the viewport that will be rendered to. Only valid during rendering
		/// @param scissorRect The scissor rectangle
		virtual void SetScissorRect(const RectInt& scissorRect) = 0;

		/// @brief Adds a semaphore that this context will wait on before executing. Only valid during rendering
		/// @param semaphore The semaphore
		virtual void AddWaitOnSemaphore(GraphicsSemaphore& semaphore) = 0;

		/// @brief Adds a semaphore that this context will signal once rendering is completed. Only valid during rendering
		/// @param semaphore The semaphore
		virtual void AddRenderCompletedSignalSemaphore(GraphicsSemaphore& semaphore) = 0;

		/// @brief Resets this context
		void Reset();

		/// @brief Begins rendering
		/// @param renderView The view to render with
		/// @param pipeline The pipeline to render with
		/// @return True if this context is ready for rendering
		bool Begin(RenderView& renderView, CompiledRenderPipeline& pipeline);

		/// @brief Begins the next render pass
		/// @return True if this context is ready for rendering
		bool BeginNextPass();

		/// @brief Ends rendering with this context
		void End();

	protected:
		/// @brief Called when this context is reset
		/// @return True if the reset was successful
		virtual bool ResetImpl() = 0;

		/// @brief Called when this context is starting the first render pass
		/// @return True if this context is ready for rendering
		virtual bool BeginImpl() = 0;

		/// @brief Called when this context should advance to the next render pass
		/// @return True if this context is ready for rendering
		virtual bool BeginNextPassImpl() = 0;

		/// @brief Called when this context has finished rendering
		virtual void EndImpl() = 0;
	};
}